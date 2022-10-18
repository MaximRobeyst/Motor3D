#pragma once
#include <DirectXMath.h>
#include <d3dx11effect.h>

#include <vector>

struct DebugVertex
{
	DirectX::XMFLOAT3 Position = {};
	DirectX::XMFLOAT4 Color = {};
};

namespace physx
{
	class PxScene;
};

class DebugRenderer final
{
public:
	~DebugRenderer();

	void Draw();
	void DrawLine(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& colorEnd);
	void DrawPhysX(physx::PxScene* pScene);

	static DebugRenderer* GetInstance();
private:
	DebugRenderer();

	static DebugRenderer* m_pInstance;

	bool InitializeBuffer(std::vector<DebugVertex>& vertices, ID3D11Buffer** pBuffer);
	void DrawVertices(std::vector<DebugVertex>& vertices, ID3D11Buffer** pBuffer);

	void BuildInputLayout();

	std::vector<DebugVertex> m_pDebugVertices{};
	std::vector<DebugVertex> m_pPhysxVertices{};

	ID3DX11Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pTechnique{ nullptr };
	ID3D11InputLayout* m_pInputLayout{ nullptr };
	ID3DX11EffectMatrixVariable* m_pMatrixVariable{ nullptr };

	ID3D11Buffer* m_pDebugVertexBuffer{};
	ID3D11Buffer* m_pPhysxVertexBuffer{};

	

};