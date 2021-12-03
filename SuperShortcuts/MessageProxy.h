/************************************************************\
* Module Name:  MessageProxy.h
* Project:      CppWindowsService
* Copyright (c) Microsoft Corporation.
*
* Implement Keyboard hook logic:
*
\***************************************************************************/

#pragma once 

#include <Windows.h>
#include <shellapi.h>
#include <string>

const UINT KEYBOARD_HOOK_EVENT = WM_USER + 1;

class MessageProxy
{
public:
    MessageProxy(LPCWCHAR szWindowClass, LPCWCHAR szWindowTile);
    ~MessageProxy();

    HWND GetWindowHandle();

    LRESULT virtual HandleMessages(UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled);
    void virtual OnDestroy();
    void virtual OnCreate();

    BOOL ShowContextMenu();
    void Close();

protected:
    int shellHookMessageID_;

private:

    BOOL InitInstance(LPCWCHAR szWindowClass, LPCWCHAR szWindowTile);
    BOOL SetSystemTray();
    void RemoveSystemTray();


    HWND m_hWnd;
    std::wstring m_szClassName;
    NOTIFYICONDATA m_niData;
    HMENU m_hMenu;
};