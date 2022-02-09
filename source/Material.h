#ifndef MATERIAL_HEADER
#define MATERIAL_HEADER

// Include Files
#include "MyEngine.h"
#include "DataTypes.h"
#include "Mesh.h"

// Material Class									
class Material
{
public:
	Material(ID3D11Device* pDevice, const std::wstring& assertFile);				// Constructor
	virtual ~Material();				// Destructor

	// Copy/move constructors and assignment operators
	Material(const Material& other);
	Material(Material&& other) noexcept = delete;
	Material& operator=(const Material& other) = delete;
	Material& operator=(Material&& other)	noexcept  = delete;
	// Member functions						
	
	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;

	ID3DX11EffectMatrixVariable* GetMatWorldViewProjMatrix() const;

	void SetDiffuseMap(Texture* pTexture);

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assertFile);

protected:
	// Private member functions								
	

	// Datamembers								
	ID3DX11Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pTechnique{ nullptr };

	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };

	std::vector<Texture*> m_pTextures;

};
#endif
