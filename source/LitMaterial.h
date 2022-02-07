#pragma once
#include "Material.h"

struct PointLight;

class LitMaterial : public Material
{
public:
	LitMaterial(ID3D11Device* pDevice, const std::wstring& assertFile);				// Constructor

	//Member Fucntions
	ID3DX11EffectMatrixVariable* GetWorldMatrix() const;
	ID3DX11EffectMatrixVariable* GetViewInverseMatrix() const;

	void SetNormalMap(Texture* pTexture);
	void SetSpecularMap(Texture* pTexture);
	void SetGlossinessMap(Texture* pTexture);

	ID3DX11EffectScalarVariable* GetNrOfLightsVariable() const;
	void SetLight(int lightIndex, const PointLight& pointLight);

private:
	//Member functions

	//Member variables
	ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable{ nullptr };
	ID3DX11EffectMatrixVariable* m_pViewInverseMatrixVariable{ nullptr };

	ID3DX11EffectVectorVariable* m_pLightDirection{ nullptr };

	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{ nullptr };

	ID3DX11EffectScalarVariable* m_pNrOfLightsVariable{ nullptr };

	ID3DX11EffectVariable* m_pLightPositionsVariable;
	ID3DX11EffectVariable* m_pLightColorsVariable;
	ID3DX11EffectVariable* m_pLightIntensityVariable;

	// find out how to used arrays and structs in hlsl/the effects framework
};

