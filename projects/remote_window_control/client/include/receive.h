#include <string>
#include <fstream>

void StreamReceiveData(char *buffer, int bytesReceived, std::string &cacheReceivedFrameData);
void RecordReceiveData(char *buffer, std::ofstream &mp4File);
void DownloadReceiveData(char *buffer, int bytesReceived, std::ofstream &mp4File , bool &isFileOpen);