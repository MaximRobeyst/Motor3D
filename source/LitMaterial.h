#pragma once
#include "Material.h"

class LitMaterial : public Material
{
public:
	LitMaterial(ID3D11Device* pDevice, const std::wstring& assertFile);				// Constructor
	~LitMaterial();

	//Member Fucntions
	ID3DX11EffectMatrixVariable* GetWorldMatrix() const;
	ID3DX11EffectMatrixVariable* GetViewInverseMatrix() const;

	void SetNormalMap(Texture* pTexture);
	void SetSpecularMap(Texture* pTexture);
	void SetGlossinessMap(Texture* pTexture);

private:
	//Member functions

	//Member variables
	ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable{ nullptr };
	ID3DX11EffectMatrixVariable* m_pViewInverseMatrixVariable{ nullptr };

	ID3DX11EffectVectorVariable* m_pLightDirection{ nullptr };

	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{ nullptr };
};

