#pragma once
#include <frame.h>
#include <log.h>
#include <fmt/core.h>
#include <opencv2/opencv.hpp>

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

using namespace cv;

extern bool StateStream;

void LoadTexture(std::vector<char> &buffer, int *width, int *height, GLuint *textureID)
{
    // Decode raw pixel data into cv::Mat
    Mat image = imdecode(buffer, IMREAD_COLOR);

    // Load from file
    int image_width = image.cols;
    int image_height = image.rows;
    uchar *image_data = image.data;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, image_data);

    *textureID = image_texture;
    *width = image_width;
    *height = image_height;
}

void DequeueFrame(std::queue<std::string> &frameDataQueue, GLuint *textureID, int *width, int *height)
{
    if (!frameDataQueue.empty())
    {
        std::string frameData = frameDataQueue.front();
        frameDataQueue.pop();
        std::vector<char> buffer(frameData.begin(), frameData.end());
        LoadTexture(buffer, width, height, textureID);
    }
}

void EnqueueFrame(std::string &cacheReceivedFrameData, std::queue<std::string> &frameDataQueue)
{
    int expectedframeID = 0;
    while (StateStream)
    {
        size_t posFrameID = cacheReceivedFrameData.find(fmt::format("[{0}:length:", expectedframeID));
        size_t posLength = cacheReceivedFrameData.find("length:");
        size_t posStartData = cacheReceivedFrameData.find(":startdataframe:");
        size_t posEndData = cacheReceivedFrameData.find(":enddataframe]");

        if (cacheReceivedFrameData == "response:streamdone")
        {
            StateStream = false;
            expectedframeID = 0;
            cacheReceivedFrameData.clear();
            PrintLog("Stopped Stream");
        }

        if ((posFrameID != std::string::npos) && (posStartData != std::string::npos) && (posEndData != std::string::npos))
        {
            int frameLength = std::stoi(cacheReceivedFrameData.substr(posLength + 7, posStartData - posLength - 7));
            std::string frameData = cacheReceivedFrameData.substr(posStartData + 16, frameLength);

            frameDataQueue.push(frameData);

            cacheReceivedFrameData.erase(posFrameID, posEndData + 14);
            expectedframeID++;
        }
    }
}
