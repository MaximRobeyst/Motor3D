#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "OBJParser.h"


// The mesh causes a memory leak that i cant find at the moment and i think it has something to with the material
// i have been looking for it for a while and i'm gonna move on for not future me should be able to find it 
Mesh::Mesh(ID3D11Device* pDevice, HWND hWnd, const std::string& filePath, Material* pMaterial)
	: m_pMatrial{ pMaterial }
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	ParseOBJ(filePath, vertices, indices);

	Initialize(pDevice, hWnd, vertices, indices);
}

Mesh::Mesh(ID3D11Device* pDevice, HWND hWnd, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Material* pMaterial)
	: m_pMatrial{ pMaterial}
{
	Initialize(pDevice, hWnd, vertices, indices);
}

Mesh::~Mesh()
{
	if(m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = 0;
	}

	if(m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = 0;
	}

	if (m_pVertexLayout)
		m_pVertexLayout->Release();

	delete m_pMatrial;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera)
{
	//Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//Set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto worldViewPorjection = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix() * m_WorldMatrix;
	m_pMatrial->GetMatWorldViewProjMatrix()->SetMatrix(&worldViewPorjection.data[0][0]);

	// Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pMatrial->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pMatrial->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
	}
}

FMatrix4 Mesh::GetWorldMatrix() const
{
	return m_WorldMatrix;
}

void Mesh::SetWorldMatrix(const FMatrix4& worldMatrix)
{
	m_WorldMatrix = worldMatrix;
}

void Mesh::Initialize(ID3D11Device* pDevice, HWND hWnd, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	// http://www.rastertek.com/dx11tut04.html

	// Create Vertex_Input Layout
	HRESULT result = S_OK;
	static const uint32_t numElement{ 4 };
	//D3D11_INPUT_ELEMENT_DESC vertexDesc[numElement]{};
	D3D11_INPUT_ELEMENT_DESC* vertexDesc = new D3D11_INPUT_ELEMENT_DESC[numElement]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "NORMAL";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TANGENT";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TEXCOORD";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 36;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_pMatrial->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElement,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pVertexLayout
	);

	if (FAILED(result))
		return;

	// Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * (uint32_t)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = vertices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;

	// Create index buffer
	m_AmountIndices = (uint32_t)indices.size();
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;
}
