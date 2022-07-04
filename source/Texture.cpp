#include "Texture.h"
#include "WICTextureLoader.h"

// Static datamembers

// Constructor(s) & Destructor
Texture::Texture(ID3D11Device* pDevice, const std::string& texturePath, ID3D11DeviceContext* pDeviceContext)
	: m_Path{texturePath}
{
	//SDL_Surface* pTexture{ nullptr };
	//pTexture = IMG_Load(texturePath.c_str());

	std::wstring path{ texturePath.begin(), texturePath.end() };
	auto hr = CreateWICTextureFromFile(pDevice, pDeviceContext, path.c_str(), &m_pResource, &m_pTextureResourceView);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"Failed to load in : " + *texturePath.c_str());
	}

	if (pDeviceContext)
		pDeviceContext->GenerateMips(m_pTextureResourceView);
	
	//desc.Width = m_pResource.get;
	//desc.Height = pTexture->h;
	//desc.MipLevels = 1;
	//desc.ArraySize = 1;
	//desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//desc.SampleDesc.Count = 1;
	//desc.SampleDesc.Quality = 0;
	//desc.Usage = D3D11_USAGE_DEFAULT;
	//desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//desc.CPUAccessFlags = 0;
	//desc.MiscFlags = 0;

	//D3D11_SUBRESOURCE_DATA initData;
	//initData.pSysMem = pTexture->pixels;
	//initData.SysMemPitch = static_cast<UINT>(pTexture->pitch);
	//initData.SysMemSlicePitch = static_cast<UINT>(pTexture->h * pTexture->pitch);
	//
	//HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pTexture);
	//if (FAILED(hr))
	//	return;
	//
	//D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	//SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//SRVDesc.Texture2D.MipLevels = 1;
	//
	//hr = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pTextureResourceView);
	//
	//if (pDeviceContext)
	//	pDeviceContext->GenerateMips(m_pTextureResourceView);
}

Texture::~Texture()
{
	if (m_pTexture)
		m_pTexture->Release();

	if (m_pResource)
		m_pResource->Release();

	if (m_pTextureResourceView)
		m_pTextureResourceView->Release();
}

Texture::Texture(const Texture& other)
{
	MyEngine::GetSingleton()->GetDeviceContext()->CopyResource(m_pTexture, other.m_pTexture);
	MyEngine::GetSingleton()->GetDeviceContext()->CopyResource(m_pResource, other.m_pResource);

	//MyEngine::GetSingleton()->GetDeviceContext()->CopyResource(m_pTextureResourceView, other.m_pTextureResourceView);
}

Texture& Texture::operator=(const Texture& other)
{
	MyEngine::GetSingleton()->GetDeviceContext()->CopyResource(m_pTexture, other.m_pTexture);
	MyEngine::GetSingleton()->GetDeviceContext()->CopyResource(m_pResource, other.m_pResource);

	return *this;
}

ID3D11ShaderResourceView* Texture::GetTextureShaderResource() const
{
	return m_pTextureResourceView;
}

std::string Texture::GetPath() const
{
	return m_Path;
}
