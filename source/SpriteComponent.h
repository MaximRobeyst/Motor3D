#pragma once
#include "Component.h"

#include <d3d11.h>
#include <d3dx11effect.h>


class TransformComponent;
class Texture;

class SpriteComponent : public IComponent
{
public:
	SpriteComponent();
	~SpriteComponent() override;

	void Start() override;
	void Render() override;
	void Update() override;

	void RenderGUI() override;

	void SetTexture(const std::string& spriteAsset);
	void SetColor(const DirectX::XMFLOAT4 color);

private:
	ID3DX11Effect* LoadShader( std::wstring filename);
	void CreateInputLayout();

	TransformComponent* m_pTransformComponent{};
	Texture* m_pTexture;

	ID3D11Resource* m_pTextureResource{};
	ID3D11ShaderResourceView* m_pTextureShaderResourceView{};

	ID3D11Buffer* m_pVertexBuffer{}, * m_pImmediateVertexBuffer{};
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};
	ID3D11InputLayout* m_pInputLayout{};

	ID3DX11EffectShaderResourceVariable* m_pEVar_TextureSRV{};
	ID3DX11EffectMatrixVariable* m_pEVar_TransformMatrix{};
	ID3DX11EffectVectorVariable* m_pEVar_TextureSize{};

	DirectX::XMFLOAT4 m_Color{1,1,1,1};
	DirectX::XMFLOAT4X4 m_Transform{};
	DirectX::XMFLOAT2 m_Pivot{};

	struct SpriteVertex
	{
		DirectX::XMFLOAT4 TransformData;
		DirectX::XMFLOAT4 TransformData2;
		DirectX::XMFLOAT4 Color{0.0f, 0.0f, 0.0f, 1.0f};

	};
};

