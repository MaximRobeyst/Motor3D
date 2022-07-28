#include "ParticleComponent.h"

#include "MyEngine.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "Utils.h"
#include "TransformComponent.h"
#include "Factory.h"
#include "Material.h"
#include "MaterialManager.h"
#include "Scene.h"
#include "Camera.h"
#include <comdef.h>

const Creator<IComponent, ParticleComponent> g_ParticleComponent{};

ParticleComponent::ParticleComponent(const std::string& /*assetFile*/, const ParticleEmmiterSettings& emmiterSettings, int particleCount)
	: m_pParticleArray{new Particle[particleCount]}
	, m_pParticleBuffer{new VertexParticle[particleCount]}
	, m_ParticleCount{particleCount}
	, m_MaxParticles{particleCount}
	, m_EmitterSettings{emmiterSettings}
	, m_pTexture{new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/smoke.png")}
	, m_pMaterial{new Material(MyEngine::GetSingleton()->GetDevice(), "Resources/ParticleRenderer.fx", "Particle_Material")}
{
}

ParticleComponent::~ParticleComponent()
{
	delete m_pMaterial;
	delete m_pTexture;

	delete m_pParticleArray;
	delete m_pParticleBuffer;
	m_pVertexBuffer->Release();
	m_pInputLayout->Release();
}

void ParticleComponent::Start()
{
	//m_pMaterial->SetDiffuseMap(m_pTexture);
	if(!m_pInputLayout)
		CreateInputLayout();
	if(!m_pVertexBuffer)
		CreateVertexBuffer();
}

void ParticleComponent::Render()
{
	auto camera = m_pGameobject->GetScene()->GetCamera();
	auto worldViewProj = camera->GetViewProjection();
	auto viewInv = camera->GetViewInv();

	m_pMaterial->GetEffect()->GetVariableByName("gWorldViewProj")->AsMatrix()->SetMatrix(&worldViewProj._11);
	m_pMaterial->GetEffect()->GetVariableByName("gViewInverse")->AsMatrix()->SetMatrix(&viewInv._11);
	m_pMaterial->GetEffect()->GetVariableByName("gParticleTexture")->AsShaderResource()->SetResource(m_pTexture->GetTextureShaderResource());

	auto pDeviceContext = MyEngine::GetSingleton()->GetDeviceContext();

	pDeviceContext->IASetInputLayout(m_pInputLayout);

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	const UINT offset = 0;
	const UINT stride = sizeof(VertexParticle);
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pMaterial->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pMaterial->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(static_cast<UINT>(m_ActiveParticles), 0);
	}
}

void ParticleComponent::Update(float dt)
{
	float particleInterval = ((m_EmitterSettings.maxEnergy + m_EmitterSettings.minEnergy) / 2) / m_MaxParticles;
	m_LastParticleSpawn += dt;
	m_ActiveParticles = 0;
	D3D11_MAPPED_SUBRESOURCE pData;
	HRESULT hResult;
	hResult = MyEngine::GetSingleton()->GetDeviceContext()->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	for (int i = 0; i < m_ParticleCount; ++i)
	{
		auto& currentParticle = m_pParticleArray[i];

		if (currentParticle.isActive)
			UpdateParticle(currentParticle, dt);
		if (!currentParticle.isActive && m_LastParticleSpawn >= particleInterval)
			SpawnParticle(currentParticle);

		m_pParticleBuffer[m_ActiveParticles] = currentParticle.vertexInfo;
		if (currentParticle.isActive)
			++m_ActiveParticles;
	}

	memcpy(pData.pData, m_pParticleBuffer, sizeof(VertexParticle) * m_ActiveParticles);
	MyEngine::GetSingleton()->GetDeviceContext()->Unmap(m_pVertexBuffer, 0);
}

void ParticleComponent::CreateVertexBuffer()
{
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexParticle) * m_ParticleCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	
	MyEngine::GetSingleton()->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_pVertexBuffer);
}

void ParticleComponent::CreateInputLayout()
{
	// Create Vertex_Input Layout
	HRESULT result = S_OK;
	const uint32_t numElement{ 4 };
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

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 28;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TEXCOORD";
	vertexDesc[3].Format = DXGI_FORMAT_R32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].SemanticIndex = 1;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_pMaterial->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
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

void ParticleComponent::UpdateParticle(Particle& particle, float dt)
{
	using namespace DirectX;

	if (!particle.isActive)
		return;

	particle.currentEnergy -= dt;
	if (particle.currentEnergy < 0.0f)
	{
		particle.isActive = false;
		return;
	}

	auto position = DirectX::XMLoadFloat3(&particle.vertexInfo.Position);

	auto velocity = DirectX::XMLoadFloat3(&particle.velocity);
	DirectX::XMStoreFloat3(&particle.vertexInfo.Position, position + (velocity * dt));

	float lifePercent = particle.currentEnergy / particle.totalEnergy;

	particle.vertexInfo.Color.w = particle.vertexInfo.Color.w * lifePercent;
	particle.vertexInfo.Color.w =  Clamp<float>(particle.vertexInfo.Color.w, 0.0f, 1.0f);

	particle.vertexInfo.size = (particle.initialSize + particle.sizeChange * (1.0f - lifePercent));
}

void ParticleComponent::SpawnParticle(Particle& p)
{
	using namespace DirectX;
	p.isActive = true;

	p.currentEnergy = randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.totalEnergy = randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);

	auto randomDirection = XMFLOAT3{ 1,0,0 };
	auto rotationMatrix = XMMatrixRotationRollPitchYaw(randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI));

	auto randomRotation{ XMVector3TransformNormal(XMLoadFloat3(&randomDirection), rotationMatrix) };
	float randomDisttance = randF(m_EmitterSettings.minEmmiterRadius, m_EmitterSettings.maxEmmiterRadius);
	auto transformPosition = m_pGameobject->GetTransform()->GetPosition();
	auto position = XMLoadFloat3(&transformPosition);

	XMStoreFloat3(&p.vertexInfo.Position, position + randomRotation * randomDisttance);

	auto randomSize = randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);
	p.vertexInfo.size = randomSize;
	p.initialSize = m_pGameobject->GetTransform()->GetScale().x;

	XMFLOAT3 newVelocity{};
	newVelocity.x = randF(m_EmitterSettings.minVelocity.x, m_EmitterSettings.maxVelocity.x);
	newVelocity.y = randF(m_EmitterSettings.minVelocity.y, m_EmitterSettings.maxVelocity.y);
	newVelocity.z = randF(m_EmitterSettings.minVelocity.z, m_EmitterSettings.maxVelocity.z);
	p.velocity = newVelocity;

	p.vertexInfo.rotation = randF(-XM_PI, XM_PI);

	p.vertexInfo.Color = m_EmitterSettings.color;
}
