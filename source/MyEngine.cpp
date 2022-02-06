// MyApplication.cpp : Defines the entry point for the application.
//
#pragma comment(lib,"d3d11.lib")    //https://stackoverflow.com/questions/27914309/lnk2001-and-lnk2019-errors-directx-unresolved-external-symbols
#pragma comment(lib, "dxgi")    //https://social.msdn.microsoft.com/Forums/en-US/d58c51e1-b144-4337-9e00-716eb9087812/linker-error-for-createdxgifactory-function?forum=vcgeneral


#include "framework.h"
#include "MyEngine.h"
#include "RGBColor.h"

#include <cassert>
#include <windowsx.h>
#include <chrono>

#include "MyApplication.h"

#ifdef _DEBUG
	#include <vld.h>
#endif // _DEBUG


//#ifdef _DEBUG
//	#include <vld.h>
//#endif

// initialize statics
HINSTANCE MyEngine::m_Instance{};
int MyEngine::m_Show{};
MyEngine* MyEngine::m_MyEnginePtr{ nullptr };

void StartHeapControl();
void DumpMemoryLeaks();

// WinMain
int APIENTRY wWinMain(_In_      HINSTANCE hInstance,
            _In_opt_   HINSTANCE hPrevInstance,
            _In_       LPWSTR    lpCmdLine,
            _In_       int       nCmdShow)
{
    MyEngine::Initialize(hInstance, nCmdShow);

    int result = MyEngine::GetSingleton()->Run(new MyApplication());

    delete MyEngine::GetSingleton();

    return result;
}

void StartHeapControl()
{
#if defined(DEBUG) | defined(_DEBUG)
	// Notify user if heap is corrupt
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Report detected leaks when the program exits
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Set a breakpoint on the specified object allocation order number
	//_CrtSetBreakAlloc( 1197 );
#endif
}

void DumpMemoryLeaks()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif
}

// WndProc function
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return MyEngine::GetSingleton()->HandleEvent(hWnd, message, wParam, lParam);
}

void MyEngine::Initialize(HINSTANCE hInstance, int nCmdShow)
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
	if (m_pApplication)
		delete m_pApplication;

	if (m_pRenderTargetView)
		m_pRenderTargetView->Release();

	if (m_pRenderTargetBuffer)
		m_pRenderTargetBuffer->Release();

	if (m_pDepthStencilView)
		m_pDepthStencilView->Release();

	if (m_pDepthStencilBuffer)
		m_pDepthStencilBuffer->Release();

	if (m_pSwapChain)
		m_pSwapChain->Release();

	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}

	if (m_pDevice)
		m_pDevice->Release();
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
                                CW_USEDEFAULT,          // default width
                                CW_USEDEFAULT,          // default height
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
    UINT width = rect.right - rect.left;
    UINT height = rect.bottom - rect.top;
	
    HRESULT result = InitializeDirectX(width, height, hWnd);
	if(SUCCEEDED(result))
	{
        m_Initialized = true;
        OutputDebugString(L"DirectX is initialized\n");
        m_pApplication->Initialize();
        SendMessageA(hWnd, WM_PAINT, 0, 0);
	}
    else
    {
        OutputDebugString(L"Failed to initialize\n");
    }

    // (5) load keyboard shortcuts, start the Windows message loop
    HACCEL hAccelTable = LoadAccelerators(m_Instance, MAKEINTRESOURCE(IDC_MYAPPLICATION));
    MSG msg;

	// Set start time
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();


	bool running = true;
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
		float maxElapsedSeconds = 0.1f;

		elapsedSeconds = elapsedSeconds < 0.1f ? elapsedSeconds : 0.1f;

		// Update current time
		t1 = t2;

		Update(elapsedSeconds);

		Render();
	}

    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    //    {
    //    }
    //}

    // (6) Close with received Quit message information
    return (int)msg.wParam;
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
        if (wParam == WA_INACTIVE) m_pApplication->Activate(false);
        else m_pApplication->Activate(true);
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

    //case WM_PAINT:
    //    {
    //        PAINTSTRUCT ps;
    //        m_hDC = BeginPaint(hWnd, &ps);
	//
    //        m_MyApplicationPtr->Paint();
	//
    //        EndPaint(hWnd, &ps);
    //    }
    //    break;

    case WM_DESTROY:
        //delete m_MyApplicationPtr;
	
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

HRESULT MyEngine::InitializeDirectX(UINT width, UINT height, HWND hWnd)
{
    //assert(m_Instance == nullptr);
	
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags != D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
    if (FAILED(result))
        result;

    result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
    if (FAILED(result))
        result;

    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    //Get the handle HWND from the SDL backbuffer
    swapChainDesc.OutputWindow = hWnd;

    //Create SwapChain and hook it into the handle of the SDL window
    result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
    if (FAILED(result))
        return result;

    D3D11_TEXTURE2D_DESC depthStencilDesc{};
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
    depthStencilViewDesc.Format = depthStencilDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
    if (FAILED(result))
        return result;

    result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
    if (FAILED(result))
        return result;

    //Create the RenderTargetView
    result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
    if (FAILED(result))
        return result;
    result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
    if (FAILED(result))
        return result;

    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    D3D11_VIEWPORT viewPort{};
    viewPort.Width = static_cast<float>(width);
    viewPort.Height = static_cast<float>(height);
    viewPort.TopLeftX = 0.f;
    viewPort.TopLeftY = 0.f;
    viewPort.MinDepth = 0.f;
    viewPort.MaxDepth = 1.f;
    m_pDeviceContext->RSSetViewports(1, &viewPort);

    return result;
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

void MyEngine::DrawString(const std::wstring& text, int left, int bottom, int width, int height)
{
}

void MyEngine::SetTitle(const std::wstring& text)
{
    SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM) text.c_str());
}

void MyEngine::SetBackground(const RGBColor& color)
{
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &color.r);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void MyEngine::Present()
{
    m_pSwapChain->Present(0, 0);
}

bool MyEngine::IsDirectXInitialized() const
{
    return m_Initialized;
}

ID3D11Device* MyEngine::GetDevice() const
{
    return m_pDevice;
}

ID3D11DeviceContext* MyEngine::GetDeviceContext() const
{
    return m_pDeviceContext;
}

HWND MyEngine::GetWindowHandle() const
{
    return m_hWnd;
}

void MyEngine::Render()
{
	PAINTSTRUCT ps;
	m_hDC = BeginPaint(m_hWnd, &ps);

	m_pApplication->Paint();

	EndPaint(m_hWnd, &ps);
}

void MyEngine::Update(float dt)
{
	m_pApplication->Update(dt);
}
