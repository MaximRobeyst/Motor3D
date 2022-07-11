#include "RenderTexture.h"
#include "MyEngine.h"

RenderTexture::RenderTexture()
{
	m_pRenderTargetTexture = 0;
	m_pRenderTargetView = 0;
	m_pShaderResourceView = 0;
}

RenderTexture::RenderTexture(const RenderTexture&)
{
}

RenderTexture::~RenderTexture()
{
	Shutdown();
}

bool RenderTexture::Initialize(float width, float height)
{
	auto pDevice = MyEngine::GetSingleton()->GetDevice();

	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = static_cast<UINT>(width);
	textureDesc.Height = static_cast<UINT>(height);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = pDevice->CreateTexture2D(&textureDesc, NULL, &m_pRenderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = pDevice->CreateRenderTargetView(m_pRenderTargetTexture, &renderTargetViewDesc, &m_pRenderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = pDevice->CreateShaderResourceView(m_pRenderTargetTexture, &shaderResourceViewDesc, &m_pShaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void RenderTexture::Shutdown()
{
	if (m_pShaderResourceView)
	{
		m_pShaderResourceView->Release();
		m_pShaderResourceView = 0;
	}

	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = 0;
	}

	if (m_pRenderTargetTexture)
	{
		m_pRenderTargetTexture->Release();
		m_pRenderTargetTexture = 0;
	}

	return;
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* pDeviceContext, ID3D11DepthStencilView* pDepthStencilVuew)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, pDepthStencilVuew);

	return;
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* pDeviceContext, ID3D11DepthStencilView* pDepthStencilView, float red, float green, float blue, float alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);

	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView()
{
	return m_pShaderResourceView;
}
