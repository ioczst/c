#pragma once
#include <screen.h>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <fmt/core.h>
#include <chrono>

using namespace cv;

bool EnableStream = false;
bool EnableRecord = false;
int ScreenWidth = 1920;
int ScreenHeight = 1080;

BITMAPINFOHEADER CreateBitmapHeader(int width, int height)
{
	BITMAPINFOHEADER bi;

	// create a bitmap
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height; // this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	return bi;
}
Mat CaptureScreenMat(HWND hwnd)
{
	Mat src;
	
	// get handles to a device context (DC)
	HDC hwindowDC = GetDC(hwnd);
	HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	// create mat object
	src.create(ScreenHeight, ScreenWidth, CV_8UC4);

	// create a bitmap
	HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, ScreenWidth, ScreenHeight);
	BITMAPINFOHEADER bi = CreateBitmapHeader(ScreenWidth, ScreenHeight);

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);

	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, ScreenWidth, ScreenHeight, hwindowDC, 0, 0, ScreenWidth, ScreenHeight, SRCCOPY); // change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, ScreenHeight, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);				   // copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}

void WriteMP4(std::queue<Mat> &frameQueue)
{
	VideoWriter videoWriter;
	videoWriter.open("./output.mp4", VideoWriter::fourcc('m', 'p', '4', 'v'), 24, Size(ScreenWidth, ScreenHeight));

	while (EnableRecord)
	{
		if (!frameQueue.empty())
		{
			Mat frameData = frameQueue.front();
			frameQueue.pop();
			videoWriter.write(frameData);
		}
	}
	videoWriter.release();
}

double Record()
{
	HWND hwndDesktop = GetDesktopWindow();

	std::queue<Mat> frameQueue;
	std::thread writeMP4Thread(WriteMP4, std::ref(frameQueue));

	auto start = std::chrono::high_resolution_clock::now();
	while (EnableRecord)
	{
		Mat frame = CaptureScreenMat(hwndDesktop);

		if (frame.empty())
			break;

		frameQueue.push(frame);
	}
	auto end = std::chrono::high_resolution_clock::now();

	writeMP4Thread.join();
	return ((std::chrono::duration<double>)(end - start)).count();
}

void StreamSingleFrame(SOCKET &clientSocket, HWND hwndDesktop, int frameID)
{
	// capture image
	Mat src = CaptureScreenMat(hwndDesktop);
	
	// encode result
	std::vector<uchar> buf;
	imencode(".jpg", src, buf);
	std::string frame = fmt::format("[{0}:length:{1}:startdataframe:{2}:enddataframe]", frameID, buf.size(), std::string(buf.begin(), buf.end()));
	send(clientSocket, frame.c_str(), frame.length(), 0);
	buf.clear();
}

double Stream(SOCKET &clientSocket)
{
	HWND hwndDesktop = GetDesktopWindow();
	std::chrono::steady_clock::time_point start, end;
	int frameID = 0;

	start = std::chrono::high_resolution_clock::now();
	while (EnableStream)
	{
		StreamSingleFrame(clientSocket, hwndDesktop, frameID);
		frameID++;
	}
	end = std::chrono::high_resolution_clock::now();

	char response[] = "response:streamdone";
	send(clientSocket, response, strlen(response), 0);
	return ((std::chrono::duration<double>)(end - start)).count();
}