#include "LitMaterial.h"

LitMaterial::LitMaterial(ID3D11Device* pDevice, const std::wstring& assertFile)
	: Material(pDevice, assertFile)
{
	m_pWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrixVariable->IsValid())
		OutputDebugStringW(L"m_pWorldMatrixVariable is invalid");

	m_pViewInverseMatrixVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
	if (!m_pViewInverseMatrixVariable->IsValid())
		OutputDebugStringW(L"m_pViewInverseMatrixVariable is invalid");

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
		OutputDebugStringW(L"m_pNormalMapVariable is invalid");

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
		OutputDebugStringW(L"m_pSpecularMapVariable is invalid");

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
		OutputDebugStringW(L"m_pGlossinessMapVariable is invalid");

	m_pLightDirection = m_pEffect->GetVariableByName("gLightDirection")->AsVector();
	if (!m_pLightDirection->IsValid())
		OutputDebugStringW(L"m_pLightDirection is invalid");


	FVector3 lightDirection{ 0.577f, -0.577f, -0.577f };
	m_pLightDirection->SetFloatVector(&lightDirection.x);
}

ID3DX11EffectMatrixVariable* LitMaterial::GetWorldMatrix() const
{
	return m_pWorldMatrixVariable;
}

ID3DX11EffectMatrixVariable* LitMaterial::GetViewInverseMatrix() const
{
	return m_pViewInverseMatrixVariable;
}

void LitMaterial::SetNormalMap(Texture* pTexture)
{
	m_pTextures.push_back(pTexture);

	if (m_pNormalMapVariable->IsValid())
		m_pNormalMapVariable->SetResource(pTexture->GetTextureShaderResource());
}

void LitMaterial::SetSpecularMap(Texture* pTexture)
{
	m_pTextures.push_back(pTexture);

	if (m_pSpecularMapVariable->IsValid())
		m_pSpecularMapVariable->SetResource(pTexture->GetTextureShaderResource());
}

void LitMaterial::SetGlossinessMap(Texture* pTexture)
{
	m_pTextures.push_back(pTexture);

	if (m_pGlossinessMapVariable->IsValid())
		m_pGlossinessMapVariable->SetResource(pTexture->GetTextureShaderResource());
}
