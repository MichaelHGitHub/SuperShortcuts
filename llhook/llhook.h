#pragma once 

#include <Windows.h>
#include "eventInfo.h"
#include "lldispatcher.h"

class __declspec(dllexport) LLHook
{
public:
    LLHook();
    virtual ~LLHook();

    bool HookKeyboard(HWND clientWindow, LLEventInfo* events, int number);
    bool HookMouse(HWND clientWindow, LLEventInfo* events, int number);

private:
    HHOOK m_keyboardHook;
    HHOOK m_mouseHook;
};