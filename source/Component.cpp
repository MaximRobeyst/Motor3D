#include "Component.h"
#include "Mesh.h"
#include "GameObject.h"

TransformComponent::TransformComponent(FVector3 pos, FVector3 rotation, FVector3 scale)
	: IComponent{0}
	, m_Position{pos}
	, m_Rotation{rotation}
	, m_Scale{scale}
{
}

void TransformComponent::Update(float dt)
{
}

FMatrix4 TransformComponent::GetWorldMatrix() const
{
	return MakeTranslation(m_Position); //* FMatrix4 { MakeRotationZYX(m_Rotation.x, m_Rotation.y, m_Rotation.z) } *FMatrix4{ MakeScale(m_Scale.x, m_Scale.y, m_Scale.z) };
}

FVector3 TransformComponent::GetPosition() const
{
	return m_Position;
}

void TransformComponent::SetPosition(FVector3 position)
{
	m_Position = position;
}

IComponent::IComponent(uint8_t componentID)
	: m_ComponentID{componentID}
{
}

void IComponent::Render(Camera* pCamera, GameObject* pGameobject)
{
}

void IComponent::Update(float dt, GameObject* pGameobject)
{
}

MeshComponent::MeshComponent(Mesh* pMesh)
	: IComponent{1}
	, m_pMesh{pMesh}
{
}

MeshComponent::~MeshComponent()
{
	delete m_pMesh;
}

void MeshComponent::Render(Camera* pCamera, GameObject* pGameobject)
{
	m_pMesh->Render(MyEngine::GetSingleton()->GetDeviceContext(), pCamera);
}

void MeshComponent::Update(float dt, GameObject* pGameobject)
{
	m_pMesh->SetWorldMatrix(pGameobject->GetComponent<TransformComponent>()->GetWorldMatrix());
}

RigidBodyComponent::RigidBodyComponent(FVector3 velocity, FVector3 acceleration, FVector3 gravity)
	: IComponent(0)
	, m_Velocity{velocity}
	, m_Acceleration{acceleration}
	, m_Gravity{gravity}
{
}

void RigidBodyComponent::Update(float dt, GameObject* pGameobject)
{
	m_Velocity += m_Acceleration * dt;
	m_Acceleration = m_Gravity * dt;

	auto pos = pGameobject->GetComponent<TransformComponent>()->GetPosition();
	pos += m_Velocity;

	pGameobject->GetComponent<TransformComponent>()->SetPosition(pos);
}

void RigidBodyComponent::UpdateTransform(TransformComponent* tc)
{
	tc->SetPosition(tc->GetPosition());
}
