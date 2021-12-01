#pragma once

#include <Windows.h>
#include <vector>
#include "eventInfo.h"

using namespace std;

class LLDispatcher
{
public:
    virtual ~LLDispatcher();

    static LLDispatcher& GetInstance();

    void RegisterKbEvents(HWND m_ClientWnd, LLEventInfo* pEvents, int number);
    void RegisterMouseEvents(HWND m_ClientWnd, LLEventInfo* pEvents, int number);

    // Return: if pass the event on to next hook
    bool DispatchKbEvent(WPARAM wParam, LPARAM lParam);
    // Return: if pass the event on to next hook
    bool DispatchMouseEvent(WPARAM wParam, LPARAM lParam);

private:
    LLDispatcher();

    HWND m_KbClientWnd;
    HWND m_MouseClientWnd;
    vector<LLEventInfo> m_KbEvents;
    vector<LLEventInfo> m_MouseEvents;
};