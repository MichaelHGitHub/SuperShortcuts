#include "stdafx.h"
#include <Windows.h>
#include "llhook.h"

namespace
{
    LRESULT CALLBACK LowLevelKeyboardProc(_In_  int nCode, _In_  WPARAM wParam, _In_  LPARAM lParam)
    {
        if (nCode == HC_ACTION)
        {
            if (!LLDispatcher::GetInstance().DispatchKbEvent(wParam, lParam))
            {
                return 1;
            }
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    LRESULT CALLBACK LowLevelMouseProc(_In_  int nCode, _In_  WPARAM wParam, _In_  LPARAM lParam)
    {
        if (nCode == HC_ACTION)
        {
            if (wParam != WM_MOUSEMOVE && wParam != WM_MOUSEWHEEL && wParam != WM_MOUSEHWHEEL)
            {
                if (!LLDispatcher::GetInstance().DispatchMouseEvent(wParam, lParam))
                {
                    return 1;
                }
            }
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
}

LLHook::LLHook() :
    m_keyboardHook(NULL),
    m_mouseHook(NULL)
{
}

LLHook::~LLHook()
{
    if (m_keyboardHook != NULL)
    {
        UnhookWindowsHookEx(m_keyboardHook);
    }

    if (m_mouseHook != NULL)
    {
        UnhookWindowsHookEx(m_mouseHook);
    }
}

bool LLHook::HookKeyboard(HWND clientWindow, LLEventInfo* pEvents, int number)
{
    bool result = false;
    if (nullptr != pEvents && 0 != number && ::IsWindow(clientWindow))
    {
        m_keyboardHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, ::GetModuleHandle(0), NULL);
        if (m_keyboardHook != NULL)
        {
            LLDispatcher::GetInstance().RegisterKbEvents(clientWindow, pEvents, number);
            result = true;
        }
    }
    return result;
}

bool LLHook::HookMouse(HWND clientWindow, LLEventInfo* pEvents, int number)
{
    bool result = false;

    if (nullptr != pEvents && 0 != number && ::IsWindow(clientWindow))
    {
        m_mouseHook = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, ::GetModuleHandle(0), NULL);
        if (m_mouseHook != NULL)
        {
            LLDispatcher::GetInstance().RegisterMouseEvents(clientWindow, pEvents, number);
            result = true;
        }
    }
    return result;
}
