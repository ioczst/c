#pragma once
#include <receive.h>
#include <log.h>
#include <fmt/core.h>
#include <winsock2.h>

extern bool StateStream;
extern bool StateRecord;
extern bool StateDownload;
extern bool StateDownload;

extern SOCKET ConnectSocket;

void StreamReceiveData(char *buffer, int bytesReceived, std::string &cacheReceivedFrameData)
{
    if (StateStream)
    {
        std::string sbuffer = std::string(buffer, bytesReceived);
        cacheReceivedFrameData.append(sbuffer);
    }
}

void DownloadReceiveData(char *buffer, int bytesReceived, std::ofstream &mp4File, bool &isFileOpen)
{
    if (StateDownload)
    {
        if (!isFileOpen)
        {
            mp4File.open("./download.mp4", std::ios::binary);
            isFileOpen = true;
        }

        mp4File.write(buffer, bytesReceived);
        unsigned long bytesAvailableInSocketBuffer;
        ioctlsocket(ConnectSocket, FIONREAD, &bytesAvailableInSocketBuffer);
#if _DEBUG
        fmt::println("Bytes received: {}", bytesReceived);
        fmt::println("Bytes available in socket buffer: {}", bytesAvailableInSocketBuffer);
#endif
        // get enough data
        if (bytesAvailableInSocketBuffer == 0)
        {
            if (isFileOpen) {
                mp4File.close();
                isFileOpen = false;
            }
            PrintLog("Video file saved as download.mp4");
            StateDownload = false;
        }
    }
}
