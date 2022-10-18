#include "DebugRenderer.h"

#include "PhysXManager.h"
#include "PhysxHelper.h"

#include "MyEngine.h"
#include "Camera.h"
#include "MyApplication.h"
#include "Material.h"
#include "Logger.h"

DebugRenderer* DebugRenderer::m_pInstance{nullptr};

void DebugRenderer::Draw()
{
	DrawVertices(m_pDebugVertices, &m_pDebugVertexBuffer);
	DrawVertices(m_pPhysxVertices, &m_pPhysxVertexBuffer);
}

void DebugRenderer::DrawLine(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& color)
{
	m_pDebugVertices.emplace_back(DebugVertex{ start, color });
	m_pDebugVertices.emplace_back(DebugVertex{ end, color });
}

void DebugRenderer::DrawPhysX(physx::PxScene* pScene)
{
	const auto pxDebugRenderer = &pScene->getRenderBuffer();
	const auto pxDebugLines = pxDebugRenderer->getLines();
	const auto pxLinesCount = pxDebugRenderer->getNbLines();

	m_pPhysxVertices.reserve(size_t(pxLinesCount) * 2);
	for (unsigned int i = 0; i < pxLinesCount; ++i)
	{
		const auto& line = pxDebugLines[i];
		m_pPhysxVertices.emplace_back(PhysxHelper::ToXMFLOAT3(line.pos0), PhysxHelper::ColorToXMFLOAT4(line.color0));
		m_pPhysxVertices.emplace_back(PhysxHelper::ToXMFLOAT3(line.pos1), PhysxHelper::ColorToXMFLOAT4(line.color1));
	}
}

DebugRenderer::~DebugRenderer()
{
	if (m_pInputLayout)
		m_pInputLayout->Release();
	if (m_pTechnique)
		m_pTechnique->Release();
	if (m_pEffect)
		m_pEffect->Release();

	m_pDebugVertices.clear();
	m_pPhysxVertices.clear();

	if(m_pDebugVertexBuffer != nullptr)
		m_pDebugVertexBuffer->Release();
	if (m_pPhysxVertexBuffer != nullptr)
		m_pPhysxVertexBuffer->Release();
}

bool DebugRenderer::InitializeBuffer(std::vector<DebugVertex>& vertices, ID3D11Buffer** pBuffer)
{
	const auto size = vertices.size();
	if (size <= 0)
		return false;

	if (*pBuffer == nullptr)
	{
		if (*pBuffer != nullptr)
			(*pBuffer)->Release();

		//Vertexbuffer
		D3D11_BUFFER_DESC buffDesc{};
		buffDesc.Usage = D3D11_USAGE_DYNAMIC;
		buffDesc.ByteWidth = sizeof(DebugVertex) * static_cast<UINT>( size);
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = vertices.data();

		HRESULT result = MyEngine::GetSingleton()->GetDevice()->CreateBuffer(&buffDesc, &initData, pBuffer);
		if (FAILED(result))
			Logger::GetInstance()->LogErrorAndBreak("Failed to create buffer");

		return true;
	}

	//Update Dynamic
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	const auto pDeviceContext = MyEngine::GetSingleton()->GetDeviceContext();

	pDeviceContext->Map(*pBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	memcpy(&static_cast<DebugVertex*>(mappedResource.pData)[0], vertices.data(), sizeof(DebugVertex) * vertices.size());
	pDeviceContext->Unmap(*pBuffer, 0);

	return true;
}

void DebugRenderer::DrawVertices(std::vector<DebugVertex>& vertices, ID3D11Buffer** pBuffer)
{
	if (vertices.size() <= 0) return;

	InitializeBuffer(vertices, pBuffer);
	const auto pDeviceContext = MyEngine::GetSingleton()->GetDeviceContext();

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
	constexpr UINT stride = sizeof(DebugVertex);
	constexpr UINT offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &*pBuffer, &stride, &offset);
	pDeviceContext->IAGetInputLayout(&m_pInputLayout);

	auto viewProj = MyEngine::GetSingleton()->GetApplication()->GetCamera()->GetViewProjectionMatrix();
	m_pMatrixVariable->SetMatrix(&viewProj._11);

	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pTechnique->GetDesc(&techDesc);

	for (unsigned int i = 0; i < techDesc.Passes; ++i)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0, pDeviceContext);
		pDeviceContext->Draw((UINT)m_pDebugVertices.size(), 0);
	}

	vertices.clear();
}

void DebugRenderer::BuildInputLayout()
{
	auto pDevice = MyEngine::GetSingleton()->GetDevice();

	// Create Vertex_Input Layout
	HRESULT result = S_OK;
	static const uint32_t numElement{ 2 };
	//D3D11_INPUT_ELEMENT_DESC vertexDesc[numElement]{};
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElement]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	D3DX11_PASS_DESC passDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElement,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout
	);

	if (FAILED(result))
		return;
}

DebugRenderer::DebugRenderer()
{
	m_pEffect = Material::LoadEffect(MyEngine::GetSingleton()->GetDevice(), L"Resources/DebugRenderer.fx");
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	BuildInputLayout();

	m_pMatrixVariable = m_pEffect->GetVariableBySemantic("WORLDVIEWPROJECTION")->AsMatrix();
	if (!m_pMatrixVariable->IsValid())
		Logger::GetInstance()->LogWarning("Debug Renderer: Invalid Shader Variable! (WVP)");
}

DebugRenderer* DebugRenderer::GetInstance()
{
	if (m_pInstance == nullptr) m_pInstance = new DebugRenderer();

	return m_pInstance;
}
