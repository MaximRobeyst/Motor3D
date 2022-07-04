#include "Material.h"

#include <cassert>
#include <sstream>

#include "Texture.h"
#include "Scene.h"
#include "MaterialManager.h"

Material::Material(ID3D11Device* pDevice, const std::string& assertFile, const std::string& name)
	: m_Name{name}
	, m_AssertFile{assertFile}
{
	std::wstring file{ m_AssertFile.begin(), m_AssertFile.end() };
	m_pEffect = LoadEffect(pDevice, file);

	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	assert(m_pTechnique->IsValid());

	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
		OutputDebugStringW(L"m_pMatWorldViewProjVariable is invalid");

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if(!m_pDiffuseMapVariable->IsValid())
		OutputDebugStringW(L"m_pDiffuseMapVariable is invalid");
}

Material::~Material()
{
	delete m_pTexture;
	m_pTexture = nullptr;

	if (m_pTechnique)
		m_pTechnique->Release();
	if (m_pEffect)
		m_pEffect->Release();
}

Material::Material(const Material& other)
{
	m_pTexture = other.m_pTexture;
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
	if (m_pTexture != nullptr) delete m_pTexture;

	m_pTexture = pTexture;

	if (m_pDiffuseMapVariable->IsValid())
		m_pDiffuseMapVariable->SetResource(pTexture->GetTextureShaderResource());
}

void Material::SetName(const std::string& name)
{
	m_Name = name;
}

std::string Material::GetName() const
{
	return m_Name;
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

void Material::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("DiffuseTexture");
	writer.String(m_pTexture->GetPath().c_str());

	writer.Key("AssertFile");
	writer.String(m_AssertFile.c_str());
}

Material* Material::Deserialize(Scene* , const rapidjson::Value& value)
{
	auto pMaterial = new Material(MyEngine::GetSingleton()->GetDevice(), value["AssertFile"].GetString(), value["Name"].GetString());

	pMaterial->SetDiffuseMap(new Texture(MyEngine::GetSingleton()->GetDevice(), value["DiffuseTexture"].GetString()));

	return pMaterial;
}
