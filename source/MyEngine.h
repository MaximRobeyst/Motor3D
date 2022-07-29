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
class DX11Renderer;
class RenderTexture;

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

	static void Initialize(HINSTANCE hInstance, HINSTANCE,  int nCmdShow);
	static MyEngine* GetSingleton();

	//-------------------------------------------------
	// API methods							
	//-------------------------------------------------
	void SetTitle(const std::wstring& text);

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
	DX11Renderer* GetRenderer() const;

	MyApplication* GetApplication() const;

	HWND GetWindowHandle() const;

	float GetWindowHeight() const;
	float GetWindowWidth() const;

	bool SetPlaying(bool playing);
	bool GetPlaying() const;

	void SetInWindow(bool value);
	bool GetInWindow() const;

private:
	void Start();
	void Render();
	void Update();

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

	bool m_Playing{false};
	bool m_InWindow{ true };
	bool m_FollowGameCamera{ false };

	DX11Renderer* m_pRenderer{};

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11Resource* m_pRenderTargetBuffer;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Texture2D* m_pRenderTargetTexture{};


	RenderTexture* m_pRenderTarget{};

	COLORREF m_Color{ RGB(255, 255, 255) };
};