#pragma once

#include "Component.h"

#include <rapidjson.h>
#include <prettywriter.h>
#include <document.h>
#include <DirectXMath.h>

class TransformComponent : public IComponent
{
public:
	TransformComponent(DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3{}, DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3{}, DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3{ 1,1,1 });

	void Start() override;
	void Update(float dt) override;

	void RenderGUI() override;

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>& writer);
	virtual void Deserialize(const rapidjson::Value&);

	DirectX::XMFLOAT4X4 GetWorldMatrix();

	DirectX::XMFLOAT3 GetPosition() const;
	void SetPosition(DirectX::XMFLOAT3 position);

	DirectX::XMFLOAT3 GetRotation() const;
	void SetRotation(DirectX::XMFLOAT3 rotation);

	DirectX::XMFLOAT3 GetScale() const;
	void SetScale(DirectX::XMFLOAT3 scale);

	void SetParent(TransformComponent* pTransformComponent);

	DirectX::XMFLOAT3& GetForward();
	DirectX::XMFLOAT3& GetRight();
	DirectX::XMFLOAT3& GetUp();

private:
	void UpdateDirections();

	bool m_Dirty{ true };

	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Rotation;
	DirectX::XMFLOAT3 m_Scale;

	DirectX::XMFLOAT4X4 m_WorldMatrix;

	DirectX::XMFLOAT3 m_Forward;
	DirectX::XMFLOAT3 m_Right;
	DirectX::XMFLOAT3 m_Up;
};


