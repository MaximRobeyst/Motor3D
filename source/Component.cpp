#include "Component.h"
#include "Mesh.h"
#include "GameObject.h"

#include <imgui.h>

TransformComponent::TransformComponent(FVector3 pos, FVector3 rotation, FVector3 scale)
	: IComponent{0}
	, m_Position{pos}
	, m_Rotation{rotation}
	, m_Scale{scale}
{
}

void TransformComponent::Update(float /*dt*/)
{
}

void TransformComponent::RenderGUI()
{
	float position[] = { m_Position.x, m_Position.y, m_Position.z };
	if (ImGui::InputFloat3("Position", position))
	{
		m_Position.x = position[0];
		m_Position.y = position[1];
		m_Position.z = position[2];
	}

	float rotation[] = { m_Rotation.x, m_Rotation.y, m_Rotation.z };
	if (ImGui::InputFloat3("Rotation", rotation))
	{
		m_Rotation.x = rotation[0];
		m_Rotation.y = rotation[1];
		m_Rotation.z = rotation[2];
	}

	float scale[] = { m_Scale.x, m_Scale.y, m_Scale.z };
	if (ImGui::InputFloat3("Scale", scale))
	{
		m_Scale.x = scale[0];
		m_Scale.y = scale[1];
		m_Scale.z = scale[2];
	}

}

FMatrix4 TransformComponent::GetWorldMatrix() const
{
	FVector3 position{};
	FVector3 rotation{};

	if (m_pGameobject->GetParent() != nullptr)
	{
		position = m_Position + m_pGameobject->GetParent()->GetTransform()->GetPosition();
		rotation = m_Rotation + m_pGameobject->GetParent()->GetTransform()->GetRotation();
	}
	else
	{
		position = m_Position;
		rotation = m_Rotation;

	}


	return MakeTranslation(position) * FMatrix4{ MakeRotationZYX(rotation.x, rotation.y, rotation.z) };//*FMatrix4{ MakeScale(m_Scale.x, m_Scale.y, m_Scale.z) };
}

FVector3 TransformComponent::GetPosition() const
{
	return m_Position;
}

void TransformComponent::SetPosition(FVector3 position)
{
	m_Position = position;
}

FVector3 TransformComponent::GetRotation() const
{
	return m_Rotation;
}

void TransformComponent::SetRotation(FVector3 rotation)
{
	m_Rotation = rotation;
}

void TransformComponent::SetParent(TransformComponent* /*pTransformComponent*/)
{
}

IComponent::IComponent(uint8_t componentID)
	: m_ComponentID{componentID}
{
}

void IComponent::Render(Camera* /*pCamera*/, GameObject* /*pGameobject*/)
{
}

void IComponent::Update(float /*dt*/, GameObject* /*pGameobject*/)
{
}

void IComponent::SetGameobject(GameObject* gameobject)
{
	m_pGameobject = gameobject;
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

void MeshComponent::Render(Camera* pCamera, GameObject* /*pGameobject*/)
{
	m_pMesh->Render(MyEngine::GetSingleton()->GetDeviceContext(), pCamera);
}

void MeshComponent::Update(float, GameObject* pGameobject)
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

Rotator::Rotator(float rotationSpeed, FVector3 axis)
	: IComponent(4)
	, m_Rotation{}
	, m_RotationSpeed{rotationSpeed}
	, m_Axis{axis}
{
}

void Rotator::Update(float dt, GameObject* pGameobject)
{
	if (!m_Enabled) return;

	m_Rotation += (ToRadians(m_RotationSpeed) * dt);

	pGameobject->GetComponent<TransformComponent>()->SetRotation(m_Axis * m_Rotation);
}

void Rotator::RenderGUI()
{
	ImGui::Checkbox("Enabled", &m_Enabled);
}
