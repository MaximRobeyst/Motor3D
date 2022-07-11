#pragma once
#include <d3d11.h>

//https://www.rastertek.com/dx11tut22.html

class RenderTexture
{
public:
	RenderTexture();
	RenderTexture(const RenderTexture&);
	~RenderTexture();

	bool Initialize(float width, float height);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	ID3D11Texture2D* m_pRenderTargetTexture;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11ShaderResourceView* m_pShaderResourceView;
};

