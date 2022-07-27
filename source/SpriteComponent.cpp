#include "SpriteComponent.h"
#include "ResourceManager.h"
#include "TransformComponent.h"
#include "MyEngine.h"
#include "Factory.h"
#include "Texture.h"

#include <DirectXMath.h>

#include <sstream>
#include <comdef.h>

const Creator<IComponent, SpriteComponent> g_TransformCreator{};

SpriteComponent::SpriteComponent()
{
}

SpriteComponent::~SpriteComponent()
{
	delete m_pTexture;

	if (m_pTextureResource)
	{
		m_pTextureResource->Release();
		m_pTextureResource = nullptr;
	}

	if (m_pTextureShaderResourceView)
	{
		m_pTextureShaderResourceView->Release();
		m_pTextureShaderResourceView = nullptr;
	}

	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}

	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}

	if (m_pImmediateVertexBuffer)
	{
		m_pImmediateVertexBuffer->Release();
		m_pImmediateVertexBuffer = nullptr;
	}

	if (m_pInputLayout)
		m_pInputLayout->Release();

	if (m_pTechnique)
		m_pTechnique->Release();

	if (m_pEffect)
		m_pEffect->Release();

}

void SpriteComponent::Start()
{
	//Effect
	m_pEffect = LoadShader(L"Resources/SpriteRenderer.fx");
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	CreateInputLayout();

	m_pEVar_TransformMatrix = (m_pEffect)->GetVariableByName("gTransform")->AsMatrix();	
	if (!(m_pEVar_TransformMatrix)->IsValid())
	{
		return;
	}
	m_pEVar_TextureSize = (m_pEffect)->GetVariableByName("gTextureSize")->AsVector();
	if (!(m_pEVar_TextureSize)->IsValid())
	{
		return;
	}
	m_pEVar_TextureSRV = (m_pEffect)->GetVariableByName("gSpriteTexture")->AsShaderResource();
	if (!(m_pEVar_TextureSize)->IsValid())
	{
		return;
	}

	//Transform Matrix
	const float scaleX = (MyEngine::GetSingleton()->GetWindowWidth() > 0) ? 2.0f / float(MyEngine::GetSingleton()->GetWindowWidth()) : 0;
	const float scaleY = (MyEngine::GetSingleton()->GetWindowHeight() > 0) ? 2.0f / float(MyEngine::GetSingleton()->GetWindowHeight()) : 0;

	m_Transform = DirectX::XMFLOAT4X4{
		scaleX, 0,       0, 0,
		0,      -scaleY, 0 ,0,
		0,      0,       1, 0,
		-1,     1,       0, 1
	};

	m_pTransformComponent = m_pGameobject->GetComponent<TransformComponent>();
}

void SpriteComponent::Render()
{
	//assert(m_pTexture != nullptr);
	if (!m_pTexture)
		return;


	auto pDevice = MyEngine::GetSingleton()->GetDevice();
	auto pDeviceContext = MyEngine::GetSingleton()->GetDeviceContext();

	if (!m_pImmediateVertexBuffer)
	{
		//Create a new buffer
		D3D11_BUFFER_DESC buffDesc;
		buffDesc.Usage = D3D11_USAGE_DYNAMIC;
		buffDesc.ByteWidth = sizeof(SpriteVertex);
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffDesc.MiscFlags = 0;
		pDevice->CreateBuffer(&buffDesc, nullptr, &m_pImmediateVertexBuffer);
	}

	HRESULT hResult = S_OK;
	SpriteVertex vertex{};
	vertex.TransformData = DirectX::XMFLOAT4{ m_pTransformComponent->GetPosition().x, m_pTransformComponent->GetPosition().y, m_pTransformComponent->GetPosition().z, m_pTransformComponent->GetRotation().z};
	vertex.TransformData2 = DirectX::XMFLOAT4{ 0,0, m_pTransformComponent->GetScale().x, m_pTransformComponent->GetScale().y };
	vertex.Color = DirectX::XMFLOAT4{1,1,1,1};

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hResult = pDeviceContext->Map(m_pImmediateVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	memcpy(mappedResource.pData, &vertex, sizeof(SpriteVertex));
	pDeviceContext->Unmap(m_pImmediateVertexBuffer, 0);

	//Set Render Pipeline
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	unsigned int stride = sizeof(SpriteVertex);
	unsigned int offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pImmediateVertexBuffer, &stride, &offset);
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//Set Texture
	m_pEVar_TextureSRV->SetResource(m_pTexture->GetTextureShaderResource());

	ID3D11Resource* pResource{};
	m_pTexture->GetTextureShaderResource()->GetResource(&pResource);

	D3D11_TEXTURE2D_DESC texDesc{};
	const auto texResource = reinterpret_cast<ID3D11Texture2D*>(pResource);
	texResource->GetDesc(&texDesc);
	texResource->Release();

	//Set Texture Size
	auto texSize = DirectX::XMFLOAT2(static_cast<float>(texDesc.Width), static_cast<float>(texDesc.Height));
	m_pEVar_TextureSize->SetFloatVector(&texSize.x);

	//Set Transform
	m_pEVar_TransformMatrix->SetMatrix(&m_Transform._11);

	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pTechnique->GetDesc(&techDesc);
	for (unsigned int i = 0; i < techDesc.Passes; ++i)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(1, 0);
	}


}

void SpriteComponent::Update(float /*dt*/)
{
}

void SpriteComponent::SetTexture(const std::string& spriteAsset)
{
	m_pTexture = ResourceManager::GetInstance()->GetTexture(spriteAsset);
}

void SpriteComponent::SetColor(const DirectX::XMFLOAT4 color)
{
	m_Color = color;
}

void SpriteComponent::CreateInputLayout()
{
	// Create Vertex_Input Layout
	HRESULT result = S_OK;
	const uint32_t numElement{ 3 };
	//D3D11_INPUT_ELEMENT_DESC vertexDesc[numElement]{};
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElement]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "POSITION";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 16;
	vertexDesc[1].SemanticIndex = 1;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "COLOR";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 32;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	result = MyEngine::GetSingleton()->GetDevice()->CreateInputLayout(
		vertexDesc,
		numElement,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout
	);

	if (FAILED(result))
	{
		_com_error err(result);
		OutputDebugStringW(err.ErrorMessage());
		return;
	}
}

ID3DX11Effect* SpriteComponent::LoadShader(std::wstring filename)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	std::wstring wFilename{ filename };
	result = D3DX11CompileEffectFromFile(wFilename.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		MyEngine().GetSingleton()->GetDevice(),
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			char* pErrors = (char*)pErrorBlob->GetBufferPointer();

			std::wstringstream ss{};
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
			ss << "EffectLoader: Failed to CreateEffectFromFile! \nPath: " << filename;

			OutputDebugStringW(ss.str().c_str());
			return nullptr;
		}
	}

	return pEffect;
}
