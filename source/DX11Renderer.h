#pragma once
#include "Renderer.h"

class DX11Renderer : public Renderer
{
public:
	DX11Renderer(HWND hwnd, UINT width, UINT height);
	virtual ~DX11Renderer();

	virtual void Initialize() override;

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
	ID3D11DepthStencilView* GetStencilView() const;

private:
	HRESULT InitializeDirectX();

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGIFactory* m_pDXGIFactory;
	IDXGISwapChain* m_pSwapChain;
	ID3D11Resource* m_pRenderTargetBuffer;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilView;
};

