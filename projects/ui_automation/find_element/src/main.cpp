#include <windows.h>
#include <uiautomation.h>
#include <iostream>
#include <atlcomcli.h>
#include <string.h>
#include <vector>
#include "process.h"

IUIAutomation *UIAutomation;

struct PropertyConditionStruct
{
    PROPERTYID propertyId;
    WCHAR *value;
};

void AndCondition(std::vector<PropertyConditionStruct> propertyConditionStructVector, IUIAutomationCondition **newCondition)
{
    UIAutomation->CreateTrueCondition(newCondition);

    for (PropertyConditionStruct propertyConditionStruct : propertyConditionStructVector)
    {
        IUIAutomationCondition *propertyCondition;
        UIAutomation->CreatePropertyCondition(propertyConditionStruct.propertyId, CComVariant(propertyConditionStruct.value), &propertyCondition);
        UIAutomation->CreateAndCondition(propertyCondition, *newCondition, newCondition);
    }
}

int wmain(int argc, wchar_t *argv[])
{

#ifdef DEBUG
    argv[1] = L"notepad.exe";
#else
    if (argc < 2)
    {
        std::cout << "No second command-line argument provided." << std::endl;
        return 1;
    }
#endif

    CoInitialize(NULL);
    CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void **)&UIAutomation);

    if (UIAutomation == NULL)
    {
        std::cerr << "Failed to create instance of UI Automation." << std::endl;
        CoUninitialize();
        return 1;
    }

    IUIAutomationElement *window;

    DWORD processID = FindProcessIdByName(argv[1]);

    HWND handleWindow = FindWindowHandleFromProcessId(processID);

    UIAutomation->ElementFromHandle(handleWindow, &window);

    if (window == NULL)
    {
        std::cerr << "Failed to get window element." << std::endl;
        UIAutomation->Release();
        CoUninitialize();
        return 1;
    }


    IUIAutomationElement *selectedElement;
    IUIAutomationCondition *condition;

    AndCondition({
            {UIA_NamePropertyId, L"Application"},
            {UIA_LocalizedControlTypePropertyId, L"menu bar"},
            {UIA_AutomationIdPropertyId, L"MenuBar"},
        },&condition);
        
    window->FindFirst(TreeScope_Children, condition, &selectedElement);
    condition->Release();

    RECT boundingRectangle;
    selectedElement->get_CurrentBoundingRectangle(&boundingRectangle);
    std::wcout << L"Left Bounding Rectangle: " << boundingRectangle.left << std::endl;
    std::wcout << L"Top Bounding Rectangle: " << boundingRectangle.top << std::endl;

    selectedElement->Release();
    window->Release();
    UIAutomation->Release();
    CoUninitialize();
    return 0;
}
