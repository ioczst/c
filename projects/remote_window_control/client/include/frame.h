#include <queue>
#include <string>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

void DequeueFrame(std::queue<std::string> &frameDataQueue, GLuint *textureID, int *width, int *height);
void EnqueueFrame(std::string &cacheReceivedFrameData, std::queue<std::string> &frameDataQueue);