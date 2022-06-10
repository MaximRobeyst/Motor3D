#pragma once
// Include Files
#include "MyEngine.h"

// Texture Class									
class Texture final
{
public:
	Texture(ID3D11Device* pDevice, const std::wstring& texturePath, ID3D11DeviceContext* pDeviceContext = nullptr);				// Constructor
	~Texture();				// Destructor

	// Copy/move constructors and assignment operators
	Texture(const Texture& other);
	Texture(Texture&& other) noexcept;
	Texture& operator=(const Texture& other);
	Texture& operator=(Texture&& other)	noexcept;

	// Member functions						
	ID3D11ShaderResourceView* GetTextureShaderResource() const;
private:
	// Private member functions								


	// Datamembers
	ID3D11Resource* m_pResource{ nullptr };
	ID3D11Texture2D* m_pTexture{ nullptr };
	ID3D11ShaderResourceView* m_pTextureResourceView{ nullptr };

};
