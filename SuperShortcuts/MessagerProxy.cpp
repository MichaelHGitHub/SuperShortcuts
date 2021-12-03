/************************************************************\
* Module Name:  MessageProxy.cpp
* Project:      CppWindowsService
* Copyright (c) Microsoft Corporation.
*
* Implement Keyboard hook logic:
*
*
\***************************************************************************/


// Register keyboard hook callback

#include "stdafx.h"
#include <shellapi.h>
#include <string>
#include "resource.h"
#include "Messages.h"
#include "MessageProxy.h"

using namespace std;

HMODULE g_hModule;

namespace
{
    wchar_t szAssObjectName[] = L"AssociatedObject";

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
}

MessageProxy::MessageProxy(LPCWCHAR szWindowClass, LPCWCHAR szWindowTile) :
    m_hWnd(NULL),
    szClassName(szWindowClass),
    shellHookMessageID_(0)
{
    InitInstance(szWindowClass, szWindowTile);
}


MessageProxy::~MessageProxy()
{
    if (::IsWindow(m_hWnd))
    {
        ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
    }

    ::UnregisterClass(szClassName.c_str(), g_hModule);
}


HWND MessageProxy::GetWindowHandle()
{
    return m_hWnd;
}


BOOL MessageProxy::InitInstance(LPCWCHAR szWindowClass, LPCWCHAR szWindowTile)
{
    BOOL ret = FALSE;

    if (szWindowClass != NULL && szWindowTile != NULL)
    {
        g_hModule = ::GetModuleHandle(NULL);

        if (g_hModule != NULL)
        {

            WNDCLASSEX wcex = { 0 };

            wcex.cbSize = sizeof(WNDCLASSEX);
            wcex.lpfnWndProc = WndProc;
            wcex.hInstance = g_hModule;
            wcex.lpszClassName = szWindowClass;
            wcex.hCursor = LoadCursor(NULL, IDC_ARROW);

            ATOM atom = RegisterClassEx(&wcex);


            if (atom != NULL || ::GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            {
                m_hWnd = ::CreateWindow(szWindowClass, szWindowTile, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, g_hModule, this);

                if (m_hWnd != NULL)
                {
                    SetSystemTray();
                    OnCreate();
                    ret = TRUE;
                }
            }
        }
    }

    return ret;
}

BOOL MessageProxy::SetSystemTray()
{
    NOTIFYICONDATA niData;
    ZeroMemory(&niData, sizeof(NOTIFYICONDATA));

    niData.cbSize = sizeof(NOTIFYICONDATA);

    niData.uID = IDI_ICON_48X48;
    niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

    niData.hIcon =
        (HICON)LoadImage(GetModuleHandle(NULL),
            MAKEINTRESOURCE(IDI_ICON_32X32),
            IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON),
            GetSystemMetrics(SM_CYSMICON),
            LR_DEFAULTCOLOR);

    niData.hWnd = m_hWnd;
    niData.uCallbackMessage = SYS_TRAY_MSG;

    // NIM_ADD adds a new tray icon
    Shell_NotifyIcon(NIM_ADD, &niData);

    DestroyIcon(niData.hIcon);

    return TRUE;

}

LRESULT MessageProxy::HandleMessages(UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled)
{
    isHandled = false;

    return FALSE;
}

void MessageProxy::OnCreate()
{
    // Map the ShellHookWindow message
    shellHookMessageID_ = ::RegisterWindowMessage(L"SHELLHOOK");

    // Register for Shell Hook messages
    if (m_hWnd != NULL)
    {
        ::RegisterShellHookWindow(m_hWnd);
    }
}

void MessageProxy::OnDestroy()
{
    if (m_hWnd != NULL)
    {
        ::DeregisterShellHookWindow(m_hWnd);
    }
}


namespace
{
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        bool isHandled = false;
        LRESULT result = 0;

        MessageProxy* pObject = NULL;
        CREATESTRUCT* cStrut = NULL;

        switch (message)
        {
        case WM_CREATE:

            cStrut = (CREATESTRUCT*)lParam;
            pObject = (MessageProxy*)(cStrut->lpCreateParams);
            ::SetProp(hWnd, szAssObjectName, pObject);

            isHandled = true;

            break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            isHandled = true;

            break;

        case WM_DESTROY:
            pObject = (MessageProxy*)::GetProp(hWnd, szAssObjectName);
            if (pObject != NULL)
            {
                pObject->OnDestroy();
            }
            ::RemoveProp(hWnd, szAssObjectName);
            PostQuitMessage(0);
            isHandled = true;

            break;
        case SYS_TRAY_MSG:

            break;
        default:
            pObject = (MessageProxy*)::GetProp(hWnd, szAssObjectName);
            if (pObject != NULL)
            {
                result = pObject->HandleMessages(message, wParam, lParam, isHandled);
            }
        }

        if (isHandled)
        {
            return result;
        }
        else
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

    }
}