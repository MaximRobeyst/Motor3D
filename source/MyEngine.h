#pragma once

#include "resource.h"

#include <string>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <wincodec.h>

class MyApplication;
struct RGBColor;

class MyEngine
{
public:
	// Constructor(s) and destructor
	MyEngine();				
	virtual ~MyEngine();		

	// Disabling default copy/move constructor and assignment operator.
	MyEngine(const MyEngine& yRef) = delete;
	MyEngine(MyEngine&& yRef) = delete;
	MyEngine& operator=(const MyEngine& yRef) = delete;
	MyEngine& operator=(MyEngine&& yRef) = delete;

	//-------------------------------------------------
	// Initialization						
	//-------------------------------------------------
	int Run(MyApplication* m_MyGamePtr);
	LRESULT HandleEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HRESULT InitializeDirectX(UINT width, UINT height, HWND hWnd);
	static void Initialize(HINSTANCE hInstance, int nCmdShow);
	static MyEngine* GetSingleton();

	//-------------------------------------------------
	// API methods							
	//-------------------------------------------------
	void SetColor(COLORREF color);
	void FillEllipse(int x1, int y1, int radiusX, int radiusY);
	void DrawLine(int x1, int y1, int x2, int y2);
	void DrawString(const std::wstring& text, int left, int bottom, int width, int height);

	void SetTitle(const std::wstring& text);

	void SetBackground(const RGBColor& color);
	void Present();

	bool IsDirectXInitialized() const;

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;

	HWND GetWindowHandle() const;

private:
	void Render();
	void Update(float dt);

	//-------------------------------------------------
	// Datamembers								
	//-------------------------------------------------
	std::wstring m_AppName, m_Title;

	HDC m_hDC;
	HWND m_hWnd;
	MyApplication* m_pApplication{nullptr};
	
	static HINSTANCE m_Instance;
	static int m_Show;
	static MyEngine* m_MyEnginePtr;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGIFactory* m_pDXGIFactory;
	IDXGISwapChain* m_pSwapChain;
	ID3D11Resource* m_pRenderTargetBuffer;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilView;

	bool m_Initialized{false};

	COLORREF m_Color{ RGB(255, 255, 255) };
};