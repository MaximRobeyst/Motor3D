#include "BallComponent.h"
#include "RigidbodyComponent.h"
#include "TransformComponent.h"

#include <GameTime.h>

void BallComponent::Start()
{
	m_pRigidbodyComponent = GetGameObject()->GetComponent<RigidBodyComponent>();
	m_pTransformComponent = GetGameObject()->GetComponent<TransformComponent>();
}

void BallComponent::Update()
{
	using namespace DirectX;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 currentPosition = m_pTransformComponent->GetPosition();

	DirectX::XMVECTOR pCurrentPosition = DirectX::XMLoadFloat3(&currentPosition);
	DirectX::XMVECTOR pVelocity = DirectX::XMLoadFloat3(&m_Velocity);

	DirectX::XMStoreFloat3(&position, pCurrentPosition + pVelocity * GameTime::GetInstance().GetElapsed());
	m_pTransformComponent->SetPosition(position);
	//m_pRigidbodyComponent->AddForce(m_Velocity);
}

void BallComponent::OnTriggerEnter(GameObject* /*pOther*/)
{
	
	m_Velocity = DirectX::XMFLOAT3{m_Velocity.x * -1.f, m_Velocity.y * -1.f, 0};
}
