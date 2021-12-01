#pragma once

#include <Windows.h>

struct _declspec (dllexport) LLEventInfo
{
    DWORD v_code;
    bool up_event;
    bool ctrl_flag;
    bool shift_flag;
    bool alt_flag;
    bool pass_on;
    DWORD msg;
    int etrax_info;

    LLEventInfo()
    {
    };
    LLEventInfo(DWORD vkCode, bool isUp, bool ctrl,
        bool shift, bool atl, bool passOn, DWORD message, int etraxInfo)
    {
         v_code = vkCode;
         up_event = isUp;
         ctrl_flag = ctrl;
         shift_flag = shift;
         alt_flag = atl;
         pass_on = passOn;
         msg = message;
         etrax_info = etraxInfo;
    }
};