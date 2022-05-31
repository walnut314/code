#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <windows.h>

void PressKey(int key)
{
    INPUT ip;

    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
 
    // Press the key
    ip.ki.wVk = key; //0x41; // virtual-key code for the "a" key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
}

void ReleaseKey()
{
    INPUT ip;

    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
 
    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

int main(int argc, char *argv[])
{
    while (1) {
        PressKey(VK_TAB);
        Sleep(20000);
        ReleaseKey();
        Sleep(20000);
    }
}

