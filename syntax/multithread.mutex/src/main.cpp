#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <string>


std::mutex mtx;
std::ofstream fileA("C:\\Users\\ADMIN\\Desktop\\test\\code\\th\\src\\a.txt");


void task(std::string str)
{
    for (int i = 0; i < 100000; i++)
    {
        mtx.lock();
        fileA << str;
        mtx.unlock();
    }
}

int main()
{
    std::thread thread1(task,"@");
    std::thread thread2(task,".");
    thread1.join();
    thread2.join();
    fileA << "end" << std::endl;

    fileA.close();
}