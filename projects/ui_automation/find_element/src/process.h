#include <windows.h>

struct FindWindowData
{
    DWORD processId;
    HWND hwnd;
};

DWORD FindProcessIdByName(const wchar_t *procname);
HWND FindWindowHandleFromProcessId(DWORD processId);