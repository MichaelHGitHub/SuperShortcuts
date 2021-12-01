#include "stdafx.h"

#include <windows.h>
#include <iostream>
#include "CommandServer.h"

namespace
{
#ifdef WIN64
	const wchar_t szClientWindowClassName[] = L"SuperShortcutsWindowClassName64";
#else
	const wchar_t szClientWindowClassName[] = L"SuperShortcutsWindowClassName";
#endif
	wchar_t szWindowTile[] = L"SuperShortcutsWindow_OPT";
	wchar_t eventName[] = L"Global\\SuperShortcutsEvent";

	HANDLE hStartEvent = NULL;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
	hStartEvent = CreateEventW(NULL, TRUE, FALSE, eventName);
	if (NULL != hStartEvent && GetLastError() == ERROR_ALREADY_EXISTS)
	{
		::CloseHandle(hStartEvent);
		hStartEvent = NULL;
		// already exist
		return 0;
	}

	CommandServer commandServer(szClientWindowClassName, szWindowTile);

	if(commandServer.IsValid())
	{
		MSG message = {0};
		while(::GetMessage(&message, NULL, 0, 0) != 0)
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}

	if (NULL != hStartEvent)
	{
		::CloseHandle(hStartEvent);
		hStartEvent = NULL;
	}
	return 0;
}
