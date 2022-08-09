// Demo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Demo.h"
#include "DemoApplication.h"


#include <MyApplication.h>
#include <chrono>
#include <ctime>
#include <MyEngine.h>


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// WinMain
int APIENTRY wWinMain(_In_      HINSTANCE hInstance,
    _In_opt_   HINSTANCE hPrevInstance,
    _In_       LPWSTR    ,
    _In_       int       nCmdShow)
{
    srand(static_cast<unsigned int>(time(NULL)));
    MyEngine::Initialize(hInstance, hPrevInstance, nCmdShow);

    int result = MyEngine::GetSingleton()->Run(new DemoApplication());

    delete MyEngine::GetSingleton();

    return result;
}