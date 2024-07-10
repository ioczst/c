#pragma once
#include <winsock2.h>
#include <queue>
#include <thread>
#include <csignal>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <fmt/core.h>

#include <log.h>
#include <receive.h>
#include <frame.h>

#pragma comment(lib, "ws2_32.lib")

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

bool StateRecord = false;
bool StateStream = false;
bool StateDownload = false;
bool StateConnect = false;
bool EnableDownload = false;

char Hostname[20] = "127.0.0.1";
char Port[10] = "8080";
std::string NotificationMessage = "";
const char *ConnectTextButton = "Connect";
char TextSend[256] = "";
int XValue = 0;
int YValue = 0;

WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct sockaddr_in ClientService;

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void CloseConnection()
{
    const char *message = "disconnect";
    send(ConnectSocket, message, strlen(message), 0);

    closesocket(ConnectSocket);
    WSACleanup();
    ConnectTextButton = "Connect";
    StateConnect = false;
}

void SignalHandler(int signum)
{
    CloseConnection();
    exit(signum);
}

void ReceiveData(std::string &cacheReceivedFrameData)
{
    std::ofstream mp4File;
    bool isFileOpen = false;
    
    while (StateConnect)
    {
        if (ConnectSocket != NULL)
        {
            char buffer[1024] = {0};
            int bytesReceived;
            bytesReceived = recv(ConnectSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived != SOCKET_ERROR)
            {
                DownloadReceiveData(buffer, bytesReceived, mp4File, isFileOpen);
                StreamReceiveData(buffer, bytesReceived, cacheReceivedFrameData);
            }
        }
    }
}

int main()
{
    std::string cacheReceivedFrameData;
    std::queue<std::string> frameDataQueue;
    GLuint imageFrameTexture = 0;
    int frameWidth = 0;
    int frameHeight = 0;
    int widthWindow = 1400;
    int heightWindow = 610;

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(widthWindow, heightWindow, "Dashboard", nullptr, nullptr);
    if (window == nullptr)
        return 1;

    signal(SIGINT, SignalHandler);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Main Panel");

        if (ImGui::CollapsingHeader("Connection"))
        {
            ImGui::InputText("Host Name", Hostname, sizeof(Hostname));
            ImGui::InputText("Port", Port, sizeof(Port));

            if (ImGui::Button(ConnectTextButton))
            {
                if (!StateConnect)
                {
                    int iResult;

                    // Initialize Winsock
                    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
                    if (iResult != 0)
                    {
                        PrintLog(fmt::format("WSAStartup failed: {0}", WSAGetLastError()));
                    }
                    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

                    if (ConnectSocket == INVALID_SOCKET)
                    {
                        PrintLog(fmt::format("Error at socket(): {0}", WSAGetLastError()));
                        WSACleanup();
                    }

                    // Set up the address structure
                    ClientService.sin_family = AF_INET;
                    ClientService.sin_addr.s_addr = inet_addr(Hostname);          // Server IP address
                    ClientService.sin_port = htons(std::stoi(std::string(Port))); // Server port

                    // Connect to server
                    iResult = connect(ConnectSocket, (SOCKADDR *)&ClientService, sizeof(ClientService));
                    if (iResult == SOCKET_ERROR)
                    {
                        PrintLog(fmt::format("Failed to connect: {0}", WSAGetLastError()));
                        CloseConnection();
                    }
                    else
                    {
                        ConnectTextButton = "Disconnect";
                        StateConnect = true;

                        std::thread thread(ReceiveData, std::ref(cacheReceivedFrameData));
                        thread.detach();
                    }
                }
                else
                {
                    CloseConnection();
                }
            }
        }

        if (StateConnect)
        {
            if (ImGui::CollapsingHeader("Notepad"))
            {
                if (ImGui::Button("Open Window"))
                {
                    const char *message = "opennotepad";
                    send(ConnectSocket, message, strlen(message), 0);
                }

                ImGui::InputText("###", TextSend, sizeof(TextSend));

                ImGui::SameLine();
                if (ImGui::Button("Send"))
                {
                    std::string message = fmt::format("sendkey,{0}", TextSend);
                    send(ConnectSocket, message.c_str(), message.length(), 0);
                }

                bool changedX = ImGui::SliderInt("X", &XValue, 0, 1000);
                bool changedY = ImGui::SliderInt("Y", &YValue, 0, 500);

                if (changedX || changedY)
                {
                    std::string message = fmt::format("setpos,{0},{1}", XValue, YValue);
                    send(ConnectSocket, message.c_str(), message.length(), 0);
                }
            }
            if (ImGui::CollapsingHeader("Screenshot"))
            {
                if (!StateRecord)
                {
                    if (ImGui::Button("Start Record"))
                    {
                        StateRecord = true;
                        const char *message = "startrecord";
                        send(ConnectSocket, message, strlen(message), 0);
                        PrintLog("Recording...");
                        EnableDownload = true;
                    }
                    if (EnableDownload)
                    {
                        ImGui::SameLine();

                        if (ImGui::Button("Download"))
                        {
                            StateDownload = true;
                            const char *message = "download";
                            send(ConnectSocket, message, strlen(message), 0);
                            PrintLog("Downloading...");
                        }
                    }
                }
                if (StateRecord)
                {
                    if (ImGui::Button("Stop Record"))
                    {
                        StateRecord = false;
                        const char *message = "stoprecord";
                        send(ConnectSocket, message, strlen(message), 0);
                        PrintLog("Stopped Record");
                    }
                }

                if (!StateStream)
                {
                    if (ImGui::Button("Start Stream"))
                    {
                        StateStream = true;
                        const char *message = "startstream";
                        send(ConnectSocket, message, strlen(message), 0);

                        std::thread readFrameThread(EnqueueFrame, std::ref(cacheReceivedFrameData), std::ref(frameDataQueue));
                        readFrameThread.detach();

                        PrintLog("Streamming...");
                    }
                }
                if (StateStream)
                {
                    if (ImGui::Button("Stop Stream"))
                    {
                        const char *message = "stopstream";
                        send(ConnectSocket, message, strlen(message), 0);
                    }
                }
            }
        }

        if (ImGui::CollapsingHeader("Status"))
        {
            ImGui::Text(NotificationMessage.c_str());
        }

        ImGui::End();

        ImGui::Begin("Screen");
        if (StateStream)
        {
            DequeueFrame(frameDataQueue, &imageFrameTexture, &frameWidth, &frameHeight);
            ImGui::Image((void *)(intptr_t)imageFrameTexture, ImVec2(frameWidth, frameHeight));
        }
        ImGui::End();
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    CloseConnection();

    return 0;
}
