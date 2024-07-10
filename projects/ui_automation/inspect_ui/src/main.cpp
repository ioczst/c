#include <windows.h>
#include <uiautomation.h>
#include <iostream>
#include <cwctype>
#include <atlcomcli.h>
#include <tlhelp32.h>
#include <string.h>
#include "process.h"

IUIAutomation *UIAutomation;

void PrintTreeLayout(int depth, bool start = true)
{
    char *sign = " ";
    if (start)
    {
        sign = "-";
    }
    for (int i = 0; i < depth; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            std::cout << sign;
        }
    }
    std::cout << "|";
}

void TraverseElements(IUIAutomationElement *pElement, int depth = 0)
{
    if (!pElement)
        return;

    IUIAutomationCondition *trueCondition;
    UIAutomation->CreateTrueCondition(&trueCondition);

    BSTR name;
    pElement->get_CurrentName(&name);
    BSTR localizedControlType;
    pElement->get_CurrentLocalizedControlType(&localizedControlType);
    BSTR automationId;
    pElement->get_CurrentAutomationId(&automationId);

    PrintTreeLayout(depth);
    std::wcout << L"Name: " << (name ? name : L"(No Name)") << std::endl;
    PrintTreeLayout(depth, false);
    std::wcout << L"LocalizedControlType: " << (localizedControlType ? localizedControlType : L"(empty)") << std::endl;
    PrintTreeLayout(depth, false);
    std::wcout << L"AutomationID: " << (automationId ? automationId : L"(empty)") << std::endl;

    // Get child elements
    IUIAutomationElementArray *children;
    pElement->FindAll(TreeScope_Children, trueCondition, &children);
    int count;
    children->get_Length(&count);

    // Recursively traverse child elements
    for (int i = 0; i < count; ++i)
    {
        IUIAutomationElement *child;
        children->GetElement(i, &child);
        TraverseElements(child, depth + 1);
        child->Release();
    }
    children->Release();
    SysFreeString(name);
    SysFreeString(localizedControlType);
    SysFreeString(automationId);
}

int wmain(int argc, wchar_t *argv[])
{
    if (argc < 2)
    {
        std::cout << "No second command-line argument provided." << std::endl;
        return 1;
    }

    CoInitialize(NULL);
    CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void **)&UIAutomation);

    if (UIAutomation == NULL)
    {
        std::cerr << "Failed to create instance of UI Automation." << std::endl;
        CoUninitialize();
        return 1;
    }

    IUIAutomationElement *pWindow;

    DWORD processID = FindProcessIdByName(argv[1]);

    HWND handleWindow = FindWindowHandleFromProcessId(processID);

    UIAutomation->ElementFromHandle(handleWindow, &pWindow);

    if (pWindow == NULL)
    {
        std::cerr << "Failed to get window element." << std::endl;
        UIAutomation->Release();
        CoUninitialize();
        return 1;
    }

    while (true)
    {
        // Check if the 'Z' key is pressed
        if (GetAsyncKeyState('Z') & 0x8000)
        {
            // std::cout << "'Z' key pressed!" << std::endl;
            system("cls");

            TraverseElements(pWindow);
        }

        // Check if the Escape key is pressed to exit the loop
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            break;
        }

        // Add a small delay to reduce CPU usage
        Sleep(10); // Sleep for 10 milliseconds
    }
    UIAutomation->Release();
    CoUninitialize();
    return 0;
}
