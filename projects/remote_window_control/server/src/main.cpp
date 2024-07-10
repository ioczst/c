#pragma once
#include <winsock2.h>
#include <windows.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <fmt/core.h>
#include <common.h>
#include <screen.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

extern bool EnableStream;
extern bool EnableRecord;

SOCKET ServerSocket;
SOCKET ClientSocket;
WSADATA WSAData;
HWND hwnd;

void DownloadThreadFunction()
{
    std::ifstream mp4file("output.mp4", std::ios::binary);

    if (!mp4file.is_open())
    { 
        std::cerr << "Error opening the file." << std::endl;
        return; 
    }
    std::vector<char> buffer(std::istreambuf_iterator<char>(mp4file), {});

    mp4file.close();

    send(ClientSocket, buffer.data(), buffer.size(), 0);
}

void RecordScreenThreadFunction()
{
    double real_duration = Record();
    PrintLog(fmt::format("Record within {0} seconds",real_duration));
    PrintLog("Video file saved as ./output.mp4");
}

void StreamScreenThreadFunction()
{
    double real_duration = Stream(ClientSocket);
    PrintLog(fmt::format("Streaming within {0} seconds",real_duration));
}

void OpenNotepadWindowController()
{
    hwnd = FindWindow(TEXT("Notepad"), NULL);
    if (hwnd == NULL)
    {
        ShellExecute(NULL, TEXT("open"), TEXT("notepad.exe"), NULL, NULL, SW_SHOWNORMAL);
        // Wait for the new window to appear
        do
        {
            Sleep(100); // Wait for 100 milliseconds
            hwnd = FindWindow(TEXT("Notepad"), NULL);
        } while (hwnd == NULL);
    }
}

void SendTextController(std::vector<std::string> tokens)
{
    const char *text = tokens[1].c_str();

    SetForegroundWindow(hwnd);

    // Simulate pressing each character key
    for (int i = 0; text[i] != '\0'; ++i)
    {
        INPUT input[2] = {0};
        input[0].type = input[1].type = INPUT_KEYBOARD;
        input[0].ki.dwFlags = input[1].ki.dwFlags = KEYEVENTF_UNICODE;

        // Set the Unicode character code
        input[0].ki.wScan = input[1].ki.wScan = text[i];

        // Press
        SendInput(1, input, sizeof(INPUT));

        // Release
        input[0].ki.dwFlags = input[1].ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(1, input, sizeof(INPUT));
    }
}

void SetWindowPositionController(std::vector<std::string> tokens)
{
    int x = std::stoi(tokens[1]);
    int y = std::stoi(tokens[2]);
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, 100, 100, SWP_NOZORDER | SWP_NOSIZE);
    SetForegroundWindow(hwnd);
}

void StartRecordScreenController()
{
    EnableRecord =  true;
    std::thread thread(RecordScreenThreadFunction);
    thread.detach();
}
void StopRecordScreenController()
{
    EnableRecord = false;
}

void StartStreamScreenController()
{
    EnableStream =  true;
    std::thread thread(StreamScreenThreadFunction);
    thread.detach();
}
void StopStreamScreenController()
{
    EnableStream = false;
}

void DownloadVideoFileController()
{
    std::thread thread(DownloadThreadFunction);
    thread.detach();
}

void Disconnect()
{
    closesocket(ClientSocket);
    PrintLog("Client disconnected");
}

void StartServer()
{
    int result = WSAStartup(MAKEWORD(2, 2), &WSAData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return;
    }

    // Create socket
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        return;
    }

    // Bind socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    if (bind(ServerSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        return;
    }

    // Listen for incoming connections
    if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        return;
    }

    PrintLog(fmt::format("Server listening on port {0}...", PORT));
}

void StopServer()
{
    closesocket(ServerSocket);
    WSACleanup();
}

void AcceptIncoming()
{
    while (true)
    {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        
        // Accept incoming connections
        ClientSocket = accept(ServerSocket, (SOCKADDR *)&clientAddr, &clientAddrSize);
        if (ClientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            return;
        }

        PrintLog("Client connected");

        while (true)
        {
            char buffer[1024];
            int bytesReceived = recv(ClientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived == SOCKET_ERROR)
            {
                std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
                closesocket(ClientSocket);
                break;
            }
            else
            {
                buffer[bytesReceived] = '\0'; // Null-terminate the received data
                std::vector<std::string> tokens = SplitString(buffer, ',');
                if (tokens.size() > 0)
                {
                    PrintLog(fmt::format("Received message: {0}", buffer));

                    std::string commandName = tokens[0];

                    if (commandName == "setpos")
                    {
                        SetWindowPositionController(tokens);
                    }
                    if (commandName == "sendkey")
                    {
                        SendTextController(tokens);
                    }
                    if (commandName == "opennotepad")
                    {
                        OpenNotepadWindowController();
                    }                  
                     if (commandName == "startrecord")
                    {
                        StartRecordScreenController();
                    }
                      if (commandName == "stoprecord")
                    {
                        StopRecordScreenController();
                    }
                    if (commandName == "startstream")
                    {
                        StartStreamScreenController();
                    }
                    if (commandName == "stopstream")
                    {
                        StopStreamScreenController();
                    }                   
                    if (commandName == "download")
                    {
                        DownloadVideoFileController();
                    }
                    if (commandName == "disconnect")
                    {
                        Disconnect();
                        break;
                    }
                }
            }
        }
    }
}

int main()
{
    StartServer();
    AcceptIncoming();
    StopServer();
    return 0;
}