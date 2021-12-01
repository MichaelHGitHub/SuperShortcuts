
#include "KeyCommand.h"

KeyCommand::KeyCommand()
{

}

KeyCommand::~KeyCommand()
{

}

void KeyCommand::Execute()
{
    KEYBDINPUT WinDown = { 0 };
    KEYBDINPUT WinUp = { 0 };
    KEYBDINPUT ShiftDown = { 0 };
    KEYBDINPUT ShiftUp = { 0 };
    KEYBDINPUT SDown = { 0 };
    KEYBDINPUT SUp = { 0 };


    INPUT keys[6] = { 0 };

    WinDown.wVk = VK_LWIN;
    WinUp.wVk = VK_LWIN;
    WinUp.dwFlags = KEYEVENTF_KEYUP;

    ShiftDown.wVk = VK_SHIFT;
    ShiftUp.wVk = VK_SHIFT;
    ShiftUp.dwFlags = KEYEVENTF_KEYUP;

    SDown.wVk = 0x53;
    SUp.wVk = 0x53;
    SUp.dwFlags = KEYEVENTF_KEYUP;


    keys[0].type = INPUT_KEYBOARD;
    keys[0].ki = WinDown;
    keys[1].type = INPUT_KEYBOARD;
    keys[1].ki = ShiftDown;
    keys[2].type = INPUT_KEYBOARD;
    keys[2].ki = SDown;
    keys[3].type = INPUT_KEYBOARD;
    keys[3].ki = SUp;
    keys[4].type = INPUT_KEYBOARD;
    keys[4].ki = ShiftUp;
    keys[5].type = INPUT_KEYBOARD;
    keys[5].ki = WinUp;

    ::SendInput(6, keys, sizeof(INPUT));
}