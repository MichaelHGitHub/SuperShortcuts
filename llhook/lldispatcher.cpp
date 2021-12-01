#include "stdafx.h"
#include "lldispatcher.h"

LLDispatcher& LLDispatcher::GetInstance()
{
    static LLDispatcher dispatcher;
    return dispatcher;
}

LLDispatcher::LLDispatcher() :
    m_KbClientWnd(nullptr),
    m_MouseClientWnd(nullptr)
{

}

LLDispatcher::~LLDispatcher()
{

}

bool LLDispatcher::DispatchKbEvent(WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;
    bool key_up = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
    bool ctrl_flag = (GetAsyncKeyState(VK_CONTROL) & 0x8000);
    bool shift_flag = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
    bool alt_flag = (GetAsyncKeyState(VK_MENU) & 0x8000);

    for (size_t i = 0; i < m_KbEvents.size(); i++)
    {
        if (m_KbEvents[i].v_code == pKeyInfo->vkCode &&
            m_KbEvents[i].up_event == key_up &&
            m_KbEvents[i].alt_flag == alt_flag &&
            m_KbEvents[i].shift_flag == shift_flag &&
            m_KbEvents[i].ctrl_flag == ctrl_flag)
        {
            ::PostMessage(m_KbClientWnd, m_KbEvents[i].msg, wParam, LPARAM(m_KbEvents[i].etrax_info));
            return m_KbEvents[i].pass_on;
        }
    }

    return true;
}

bool LLDispatcher::DispatchMouseEvent(WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT* pMouseInfo = (MSLLHOOKSTRUCT*)lParam;
    bool ctrl_flag = (GetAsyncKeyState(VK_CONTROL) & 0x8000);
    bool shift_flag = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
    bool alt_flag = (GetAsyncKeyState(VK_MENU) & 0x8000);

    for (size_t i = 0; i < m_MouseEvents.size(); i++)
    {
        if (m_MouseEvents[i].v_code == wParam &&
            m_MouseEvents[i].alt_flag == alt_flag &&
            m_MouseEvents[i].shift_flag == shift_flag &&
            m_MouseEvents[i].ctrl_flag == ctrl_flag)
        {
            ::PostMessage(m_MouseClientWnd, m_MouseEvents[i].msg, (WPARAM)(pMouseInfo->mouseData), LPARAM(m_KbEvents[i].etrax_info));
            return m_MouseEvents[i].pass_on;
        }
    }
    return true;
}

void LLDispatcher::RegisterKbEvents(HWND clientWnd, LLEventInfo* pEvents, int number)
{
    m_KbClientWnd = clientWnd;
    for (int i = 0; i < number; i++)
    {
        m_KbEvents.push_back(*(pEvents + i));
    }
}

void LLDispatcher::RegisterMouseEvents(HWND clientWnd, LLEventInfo* pEvents, int number)
{
    m_MouseClientWnd = clientWnd;
    for (int i = 0; i < number; i++)
    {
        m_MouseEvents.push_back(*(pEvents + i));
    }
}