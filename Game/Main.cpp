#pragma comment(lib,"d3d11.lib")    //https://stackoverflow.com/questions/27914309/lnk2001-and-lnk2019-errors-directx-unresolved-external-symbols
#pragma comment(lib, "dxgi")    //https://social.msdn.microsoft.com/Forums/en-US/d58c51e1-b144-4337-9e00-716eb9087812/linker-error-for-createdxgifactory-function?forum=vcgeneral

#include <cassert>
#include <windowsx.h>
#include <chrono>
#include <ctime>

#ifdef _DEBUG
#include <vld.h>
#endif // _DEBUG

// WinMain
int APIENTRY wWinMain(_In_      HINSTANCE hInstance,
    _In_opt_   HINSTANCE hPrevInstance,
    _In_       LPWSTR    lpCmdLine,
    _In_       int       nCmdShow)
{
    srand(time(NULL));
    MyEngine::Initialize(hInstance, nCmdShow);

    int result = MyEngine::GetSingleton()->Run(new MyApplication());

    delete MyEngine::GetSingleton();

    return result;
}