

#include "stdafx.h"

#include <Windows.h>
#include "CommandServer.h"
#include <llhook.h>

const DWORD MSG_KEY_EVENT = WM_USER + 1;
const DWORD MSG_MOUSE_EVENT = WM_USER + 2;

const int MSG_NON_ACTION = 0;
const int MSG_SNIP_TOOL_SHOOTCUT = 1;
const int MSG_WEB_SEARCH_CMD = 2;

CommandServer::CommandServer(LPCWCHAR szWindowClass, LPCWCHAR szWindowTile) :
    MessageProxy(szWindowClass, szWindowTile),
    isValid(false)
{
    HookKeyEvents();
    HookMouseEvents();
}


CommandServer::~CommandServer()
{
}

void CommandServer::HookKeyEvents()
{
    if (::IsWindow(GetWindowHandle()))
    {
        LLEventInfo key_info[4] = {};

        // Hook ctrl + LWIN
        key_info[0] = LLEventInfo(VK_LWIN, false, true, false, false, false, MSG_KEY_EVENT, MSG_SNIP_TOOL_SHOOTCUT);
        key_info[1] = LLEventInfo(VK_LWIN, true, true, false, false, false, MSG_KEY_EVENT, MSG_NON_ACTION);
        // Hook ctrl + B key
        key_info[2] = LLEventInfo(0x42, false, true, false, false, false, MSG_KEY_EVENT, MSG_WEB_SEARCH_CMD);
        key_info[3] = LLEventInfo(0x42, true, true, false, false, false, MSG_KEY_EVENT, MSG_NON_ACTION);

        isValid = m_llHook.HookKeyboard(GetWindowHandle(), key_info, ARRAYSIZE(key_info));
    }
}

void CommandServer::HookMouseEvents()
{
    if (::IsWindow(GetWindowHandle()))
    {
        LLEventInfo button_info[2] = {};

        // Hook Middle button, pass on
        button_info[0] = LLEventInfo(WM_MBUTTONDOWN, false, true, false, false, true, MSG_MOUSE_EVENT, MSG_NON_ACTION);
        button_info[1] = LLEventInfo(WM_MBUTTONUP, true, true, false, false, true, MSG_MOUSE_EVENT, MSG_NON_ACTION);

        isValid = m_llHook.HookMouse(GetWindowHandle(), button_info, ARRAYSIZE(button_info));
    }
}


LRESULT CommandServer::HandleMessages(UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled)
{
    LRESULT result = FALSE;
    switch (msg)
    {
    case MSG_KEY_EVENT:
        switch ((int)lParam)
        {
            case MSG_SNIP_TOOL_SHOOTCUT:
                printCommand.Execute();
                isHandled = true;
                break;
            case MSG_WEB_SEARCH_CMD:
                isHandled = true;
                break;
            case MSG_NON_ACTION:
                isHandled = true;
                break;
            default:
                break;
        }

    case MSG_MOUSE_EVENT:
        isHandled = true;
        break;
    default:
        break;
    }
    return result;
}

bool CommandServer::IsValid()
{
    return isValid;
}
