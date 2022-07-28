#pragma once
#include "Component.h"
#include <d3d11.h>

struct ParticleEmmiterSettings
{
	float minSize{ .1f };
	float maxSize{ 2.f };

	float minEnergy{ 1.f };
	float maxEnergy{ 2.f };

	float minEmmiterRadius{ 0.f };
	float maxEmmiterRadius{ 0.f };

	float minScale{ 1.f };
	float maxScale{ 1.f };

	DirectX::XMFLOAT3 minVelocity{0,1,0};
	DirectX::XMFLOAT3 maxVelocity{0,1,0};
	DirectX::XMFLOAT4 color{ DirectX::XMFLOAT4{1.f, 1.f, 1.f, 1.f} };
};

struct VertexParticle
{
	DirectX::XMFLOAT3 Position{};
	DirectX::XMFLOAT4 Color{ 1,1,1,1 };
	float size{ 1.f };
	float rotation{ 0.f };
};

class Texture;

struct Particle
{
	VertexParticle vertexInfo{};
	DirectX::XMFLOAT3 velocity{};

	bool isActive{ false };

	float totalEnergy{};
	float currentEnergy{};

	float initialSize{};
	float sizeChange{};
};

class Material;
class ParticleComponent : public IComponent
{
public:
	ParticleComponent() = default;
	ParticleComponent(const std::string& assetFile, const ParticleEmmiterSettings& emmiterSettings, int particleCount = 50);
	~ParticleComponent();

	void Start() override;
	void Render() override;
	void Update(float) override;

private:
	void CreateVertexBuffer();
	void CreateInputLayout();

	void UpdateParticle(Particle& particle, float dt);
	void SpawnParticle(Particle& particle);

	Texture* m_pTexture{};

	Particle* m_pParticleArray{};
	Material* m_pMaterial{};
	VertexParticle* m_pParticleBuffer{};

	int m_ParticleCount{};
	int m_MaxParticles{};
	int m_ActiveParticles{};

	float m_LastParticleSpawn{};

	ParticleEmmiterSettings m_EmitterSettings{};
	ID3D11InputLayout* m_pInputLayout{};
	ID3D11Buffer* m_pVertexBuffer{}; //The vertex buffer, containing ParticleVertex information for each Particle

};

