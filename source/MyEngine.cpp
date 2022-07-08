// MyApplication.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "MyEngine.h"

#include <cassert>
#include <windowsx.h>
#include <chrono>
#include <ctime>

#include "MyApplication.h"
#include "DX11Renderer.h"

#include <imgui.h>
#include <backends\imgui_impl_dx11.h>
#include <backends\imgui_impl_win32.h>

#ifdef _DEBUG
	#include <vld.h>
#endif // _DEBUG
#include <backends\imgui_impl_win32.cpp>


// initialize statics
HINSTANCE MyEngine::m_Instance{};
int MyEngine::m_Show{};
MyEngine* MyEngine::m_MyEnginePtr{ nullptr };

// WndProc function
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    return MyEngine::GetSingleton()->HandleEvent(hWnd, message, wParam, lParam);
}

void MyEngine::Initialize(HINSTANCE hInstance, HINSTANCE, int nCmdShow)
{
    m_Instance = hInstance;
    m_Show = nCmdShow;
}

MyEngine* MyEngine::GetSingleton()
{
    if (m_MyEnginePtr == nullptr) m_MyEnginePtr = new MyEngine();
    
    return m_MyEnginePtr;
}

// constructor
MyEngine::MyEngine()
{
    // (1) Custom variables
    m_AppName = L"MyApplication";
    m_Title = L"My Application";

    // (2) Register the window class (= choose the type of window you want)
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_Instance;
    wcex.hIcon = LoadIcon(m_Instance, MAKEINTRESOURCE(IDI_MYAPPLICATION));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYFIRSTAPPLICATION);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = m_AppName.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MYAPPLICATION));

    RegisterClassExW(&wcex); 
}

MyEngine::~MyEngine()
{
    delete m_pRenderer;

	if (m_pApplication)
		delete m_pApplication;
}

int MyEngine::Run(MyApplication* applicationPtr)
{
    m_pApplication = applicationPtr;

    // (3) Create the window, store the handle, check if all went well
    HWND hWnd = CreateWindowW(  m_AppName.c_str(),
                                m_Title.c_str(),        // window title
                                WS_OVERLAPPEDWINDOW,    // window style
                                CW_USEDEFAULT,          // default x
                                CW_USEDEFAULT,          // default y
                                1280,          // default width
                                720,          // default height
                                nullptr,
                                nullptr,
                                m_Instance,
                                nullptr);

    if (!hWnd)
    {
        MessageBox(NULL, L"Failure to create program window", L"Error", MB_ICONERROR);

        // this would be a nice place for a throw
    }

    // (4) Set the window to show
    ShowWindow(hWnd, m_Show);	    // set the window to show mode
    UpdateWindow(hWnd);	            // update

    RECT rect;
    GetWindowRect(hWnd, &rect);
    //UINT width = rect.right - rect.left;
    //UINT height = rect.bottom - rect.top;

    m_pRenderer = new DX11Renderer(hWnd, 1280, 720);
    m_pRenderer->Initialize();

    m_pSwapChain = m_pRenderer->GetSwapChain();
    m_pDevice = m_pRenderer->GetDevice();
    m_pDeviceContext = m_pRenderer->GetDeviceContext();
    m_pRenderTargetView = m_pRenderer->GetRenderTarget();
    m_pDepthStencilView = m_pRenderer->GetStencilView();

    OutputDebugString(L"DirectX is initialized\n");
    m_pApplication->Initialize();
    SendMessageA(hWnd, WM_PAINT, 0, 0);

    // (5) load keyboard shortcuts, start the Windows message loop
    //HACCEL hAccelTable = LoadAccelerators(m_Instance, MAKEINTRESOURCE(IDC_MYAPPLICATION));
    MSG msg{};

	// Set start time
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();


	bool running = true;
#ifdef _DEBUG
    Start();

    while (running)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                running = false;
        }
        // Get current time
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

        // Calculate elapsed time
        float elapsedSeconds = std::chrono::duration<float>(t2 - t1).count();

        elapsedSeconds = elapsedSeconds < 0.1f ? elapsedSeconds : 0.1f;

        // Update current time
        t1 = t2;

        if(m_Playing)
            Update(elapsedSeconds);

        Render();
    }

#else
    Start();

    while (running)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                running = false;
        }
        // Get current time
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

        // Calculate elapsed time
        float elapsedSeconds = std::chrono::duration<float>(t2 - t1).count();

        elapsedSeconds = elapsedSeconds < 0.1f ? elapsedSeconds : 0.1f;

        // Update current time
        t1 = t2;

        Update(elapsedSeconds);

        Render();
    }
#endif // 



    // (6) Close with received Quit message information
    return static_cast<int>(msg.wParam);
}

LRESULT MyEngine::HandleEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    m_hWnd = hWnd;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_ACTIVATE:
        break;

    case WM_LBUTTONUP:
        m_pApplication->LeftMouseButtonAction(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), true);
        InvalidateRect(hWnd, nullptr, true);
        break;
	case WM_LBUTTONDOWN:
		m_pApplication->LeftMouseButtonAction(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), false);
		InvalidateRect(hWnd, nullptr, true);
    case WM_RBUTTONUP:
        m_pApplication->RightMouseButtonAction(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), true);
        InvalidateRect(hWnd, nullptr, true);
        break;
	case WM_KEYDOWN:
		m_pApplication->KeyDown(wParam);
		break;
	case WM_KEYUP:
		m_pApplication->KeyUp(wParam);
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void MyEngine::SetColor(COLORREF color)
{
    m_Color = color;
}

void MyEngine::FillEllipse(int x1, int y1, int radiusX, int radiusY)
{
    HBRUSH brush = CreateSolidBrush(m_Color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(m_hDC, brush);

    Ellipse(m_hDC, x1 - radiusX / 2, y1 - radiusY / 2, x1 + radiusX / 2, y1 + radiusY / 2);

    SelectObject(m_hDC, oldBrush);
    DeleteObject(brush);
}

void MyEngine::DrawLine(int x1, int y1, int x2, int y2)
{
    HPEN pen = CreatePen(PS_SOLID, 1, m_Color);
    HPEN oldPen = (HPEN)SelectObject(m_hDC, pen);

    MoveToEx(m_hDC, x1, y1, 0);
    LineTo(m_hDC, x2, y2);

    SelectObject(m_hDC, oldPen);
    DeleteObject(pen);
}

void MyEngine::DrawString(const std::wstring& , int , int , int , int )
{
}

void MyEngine::SetTitle(const std::wstring& text)
{
    SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM) text.c_str());
}

ID3D11Device* MyEngine::GetDevice() const
{
    return m_pDevice;
}

ID3D11DeviceContext* MyEngine::GetDeviceContext() const
{
    return m_pDeviceContext;
}

DX11Renderer* MyEngine::GetRenderer() const
{
    return m_pRenderer;
}

HWND MyEngine::GetWindowHandle() const
{
    return m_hWnd;
}

float MyEngine::GetWindowHeight() const
{
    RECT rect;
    GetWindowRect(GetWindowHandle(), &rect);
    float height = static_cast<float>(rect.bottom - rect.top);
    return height;
}

float MyEngine::GetWindowWidth() const
{
    RECT rect;
    GetWindowRect(GetWindowHandle(), &rect);
    float width = static_cast<float>(rect.right - rect.left);
    return width;
}

bool MyEngine::SetPlaying(bool playing)
{
    m_Playing = playing;
    if (playing)
        Start();
    return m_Playing;
}

bool MyEngine::GetPlaying() const
{
    return m_Playing;
}

void MyEngine::Start()
{
    m_pApplication->Start();
}

void MyEngine::Render()
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clear_color_with_alpha);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    m_pApplication->Render();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
    m_pApplication->RenderGUI();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    m_pSwapChain->Present(0, 0);
}

void MyEngine::Update(float dt)
{
	m_pApplication->Update(dt);
}
