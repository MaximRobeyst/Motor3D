#include "Material.h"

#include <cassert>
#include <sstream>

Material::Material(ID3D11Device* pDevice, const std::wstring& assertFile)
{
	m_pEffect = LoadEffect(pDevice, assertFile);

	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	assert(m_pTechnique->IsValid());

	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	assert(m_pMatWorldViewProjVariable->IsValid());

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	assert(m_pDiffuseMapVariable->IsValid());
}

Material::~Material()
{
	if (m_pTechnique)
		m_pTechnique->Release();
	if (m_pEffect)
		m_pEffect->Release();
}

ID3DX11Effect* Material::GetEffect() const
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Material::GetTechnique() const
{
	return m_pTechnique;
}

ID3DX11EffectMatrixVariable* Material::GetMatWorldViewProjMatrix() const
{
	return m_pMatWorldViewProjVariable;
}

void Material::SetDiffuseMap(Texture* pTexture)
{
	if (m_pDiffuseMapVariable->IsValid())
		m_pDiffuseMapVariable->SetResource(pTexture->GetTextureShaderResource());
}

ID3DX11Effect* Material::LoadEffect(ID3D11Device* pDevice, const std::wstring& assertFile)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assertFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			char* pErrors = (char*)pErrorBlob->GetBufferPointer();

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			OutputDebugStringW(ss.str().c_str());
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile! \nPath: " << assertFile;

			OutputDebugStringW(ss.str().c_str());
			return nullptr;
		}
	}

	return pEffect;
}
