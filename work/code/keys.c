#include <windows.h>
#include <stdio.h>

// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

HHOOK _k_hook;
LRESULT __stdcall k_Callback1(int nCode, WPARAM wParam, LPARAM lParam)
{
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;

    if (wParam == WM_KEYDOWN && nCode == HC_ACTION)
    {
        switch (key->vkCode)
        {
        case VK_F1:
            puts("F1");
            break;
        case VK_F2:
            puts("F2");
            break;
        case VK_F3:
            puts("F3");
            break;
        case VK_F4:
            puts("F4");
            break;

        default:
            putchar(key->vkCode);
            putchar('\n');
            break;
            
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


int main()
{
    _k_hook = SetWindowsHookExA(WH_KEYBOARD_LL, k_Callback1, NULL, 0);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {

    }
    if (_k_hook)
        UnhookWindowsHookEx(_k_hook);
    return TRUE;
}

