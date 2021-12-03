#pragma once

#include <Windows.h>

namespace
{
#ifdef WIN64
    const wchar_t szClientWindowClassName[] = L"SuperShortcutsWindowClassName64";
#else
    const wchar_t szClientWindowClassName[] = L"SuperShortcutsWindowClassName";
#endif
    wchar_t szWindowTile[] = L"SuperShortcutsWindow_OPT";
    wchar_t eventName[] = L"Global\\SuperShortcutsEvent";

    wchar_t sysTrayTip[] = L"Super Shortcut";

}