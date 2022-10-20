#pragma once
#include <Component.h>

class RigidBodyComponent;
class BallComponent : public IComponent
{
public:
	BallComponent() = default;
	~BallComponent() = default;

	void Start() override;
	void Update() override;

	void OnTriggerEnter(GameObject* pOther) override;

private:
	DirectX::XMFLOAT3 m_Velocity{0,1,0};
	RigidBodyComponent* m_pRigidbodyComponent{};
	TransformComponent* m_pTransformComponent{};
};

