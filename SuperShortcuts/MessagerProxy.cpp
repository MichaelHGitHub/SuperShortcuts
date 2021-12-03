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

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "stdafx.h"
#include <strsafe.h>
#include <commctrl.h>
#include <Windowsx.h>
#include <string>
#include "resource.h"
#include "Consts.h"
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
    m_szClassName(szWindowClass),
    shellHookMessageID_(0),
    m_hMenu(NULL)
{
    InitInstance(szWindowClass, szWindowTile);
}


MessageProxy::~MessageProxy()
{
    if (::IsWindow(m_hWnd))
    {
        ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
    }

    ::UnregisterClass(m_szClassName.c_str(), g_hModule);
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
    BOOL result = TRUE;

    ZeroMemory(&m_niData, sizeof(NOTIFYICONDATA));

    m_niData.cbSize = sizeof(NOTIFYICONDATA);
    m_niData.uID = IDI_ICON_48X48;
    m_niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_niData.hWnd = m_hWnd;
    m_niData.uCallbackMessage = SYS_TRAY_MSG;
    wcscpy_s(m_niData.szTip, sizeof(m_niData.szTip) / sizeof(wchar_t), sysTrayTip);

    // Load the icon for high DPI.
    HRESULT hr = LoadIconMetric(GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDI_ICON_48X48), LIM_SMALL, &(m_niData.hIcon));

    if (SUCCEEDED(hr))
    {
        // NIM_ADD adds a new tray icon
        result = Shell_NotifyIcon(NIM_ADD, &m_niData);
    }
    else
    {
        result = FALSE;
    }

    if (0 != m_niData.hIcon)
    {
        DestroyIcon(m_niData.hIcon);
    }

    m_hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_POPUPMENU));

    return result;
}

void MessageProxy::RemoveSystemTray()
{
    if (NULL != m_hMenu)
    {
        DestroyMenu(m_hMenu);
    }

    Shell_NotifyIcon(NIM_DELETE, &m_niData);
}

BOOL MessageProxy::ShowContextMenu()
{
    BOOL result = TRUE;

    HMENU hMenu = GetSubMenu(m_hMenu, 0);
    POINT p = {};
    GetCursorPos(&p);
    SetForegroundWindow(m_hWnd);

    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON,
        p.x, p.y, 0, m_hWnd, NULL);

    return result;
}

LRESULT MessageProxy::HandleMessages(UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled)
{
    isHandled = false;

    return FALSE;
}

void MessageProxy::OnCreate()
{
    SetSystemTray();

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
    RemoveSystemTray();

    if (m_hWnd != NULL)
    {
        ::DeregisterShellHookWindow(m_hWnd);
    }
}

void MessageProxy::Close()
{
    ::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
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

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDM_SETTING:
                isHandled = true;
                    break;
            case IDM_CLOSE:
                pObject = (MessageProxy*)::GetProp(hWnd, szAssObjectName);
                pObject->Close();
                isHandled = true;
                break;
            default:
                break;
            }
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
            switch (LOWORD(lParam))
            {
            case WM_RBUTTONDOWN:
            case WM_CONTEXTMENU:
                pObject = (MessageProxy*)::GetProp(hWnd, szAssObjectName);
                pObject->ShowContextMenu();
                isHandled = true;
            }
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