#include "process.h"
#include <uiautomation.h>
#include <tlhelp32.h>


DWORD FindProcessIdByName(const wchar_t *procname)
{
    HANDLE hSnapshot;
    PROCESSENTRY32W pe;
    int pid = 0;
    BOOL hResult;

    // snapshot of all processes in the system
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
        return 0;

    // initializing size: needed for using Process32First
    pe.dwSize = sizeof(PROCESSENTRY32W);

    // info about first process encountered in a system snapshot
    hResult = Process32FirstW(hSnapshot, &pe);

    // retrieve information about the processes
    // and exit if unsuccessful
    while (hResult)
    {
        // if we find the process: return process ID
        if (wcscmp(procname, pe.szExeFile) == 0)
        {
            pid = pe.th32ProcessID;
            break;
        }
        hResult = Process32NextW(hSnapshot, &pe);
    }

    // closes an open handle (CreateToolhelp32Snapshot)
    CloseHandle(hSnapshot);
    return pid;
}



BOOL IsMainMindow(HWND hwnd)
{
    return GetWindow(hwnd, GW_OWNER) == (HWND)0 && IsWindowVisible(hwnd);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    FindWindowData &data = *reinterpret_cast<FindWindowData *>(lParam);
    DWORD windowProcessId;
    GetWindowThreadProcessId(hwnd, &windowProcessId);

    if ((windowProcessId == data.processId) && IsMainMindow(hwnd))
    {
        data.hwnd = hwnd;
        return FALSE; // Stop enumeration
    }
    return TRUE; // Continue enumeration
}

HWND FindWindowHandleFromProcessId(DWORD processId)
{
    FindWindowData data = {processId, nullptr};
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
    return data.hwnd;
}