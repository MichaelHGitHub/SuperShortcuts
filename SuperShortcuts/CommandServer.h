
#pragma once

#include<Windows.h>
#include <llhook.h>
#include "MessageProxy.h"
#include "KeyCommand.h"

class CommandServer : public MessageProxy
{
public:
	CommandServer(LPCWCHAR szWindowClass, LPCWCHAR szWindowTile);
	~CommandServer();

	LRESULT virtual HandleMessages(UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled);
	bool IsValid();
private:

	void HookKeyEvents();
	void HookMouseEvents();

	KeyCommand printCommand;
	LLHook m_llHook;
	bool isValid;
};