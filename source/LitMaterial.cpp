#include "LitMaterial.h"
#include "Light.h"

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

	m_pNrOfLightsVariable = m_pEffect->GetVariableByName("gNrOfLights")->AsScalar();
	if (!m_pNrOfLightsVariable->IsValid())
		OutputDebugStringW(L"m_pNrOfLightsVariable is invalid");

	m_pLightPositionsVariable = m_pEffect->GetVariableByName("gPointLightPositions");
	if(!m_pNrOfLightsVariable->IsValid())
		OutputDebugStringW(L"m_pLightPositionsVariable is invalid");

	m_pLightColorsVariable = m_pEffect->GetVariableByName("gPointLightColors");
	if (!m_pLightColorsVariable->IsValid())
		OutputDebugStringW(L"m_pLightColorsVariable is invalid");

	m_pLightIntensityVariable = m_pEffect->GetVariableByName("gPointLightsIntensity");
	if(!m_pLightIntensityVariable->IsValid())
		OutputDebugStringW(L"m_pLightIntensityVariable is invalid");

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

ID3DX11EffectScalarVariable* LitMaterial::GetNrOfLightsVariable() const
{
	return m_pNrOfLightsVariable;
}

void LitMaterial::SetLight(int lightIndex, const PointLight& pointLight)
{
	m_pLightPositionsVariable->GetElement(lightIndex)->AsVector()->SetFloatVector(&pointLight.position.x);
	m_pLightColorsVariable->GetElement(lightIndex)->AsVector()->SetFloatVector(&pointLight.color.x);
	m_pLightIntensityVariable->GetElement(lightIndex)->AsScalar()->SetFloat(pointLight.intensity);
}
