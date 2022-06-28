#include "Component.h"
#include "Mesh.h"
#include "GameObject.h"

#include <imgui.h>
#include "Utils.h"
#include "Factory.h"
#include "ResourceManager.h"

const Creator<IComponent, TransformComponent> g_TransformCreator{};
const Creator<IComponent, RigidBodyComponent> g_RigidbodyCreator{};
const Creator<IComponent, MeshComponent> g_MeshComponent{};
const Creator<IComponent, Rotator> g_RotatorComponent{};

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

	float rotation[] = { m_Rotation.x * static_cast<float>( TO_REGREES), m_Rotation.y * static_cast<float>(TO_REGREES), m_Rotation.z * static_cast<float>(TO_REGREES) };
	if (ImGui::InputFloat3("Rotation", rotation))
	{
		m_Rotation.x = rotation[0] * static_cast<float>(TO_RADIANS);
		m_Rotation.y = rotation[1] * static_cast<float>(TO_RADIANS);
		m_Rotation.z = rotation[2] * static_cast<float>(TO_RADIANS);
	}

	float scale[] = { m_Scale.x, m_Scale.y, m_Scale.z };
	if (ImGui::InputFloat3("Scale", scale))
	{
		m_Scale.x = scale[0];
		m_Scale.y = scale[1];
		m_Scale.z = scale[2];
	}

}

void TransformComponent::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("Position");
	writer.StartArray();
	writer.Double(static_cast<double>(m_Position.x));
	writer.Double(static_cast<double>(m_Position.y));
	writer.Double(static_cast<double>(m_Position.z));
	writer.EndArray();

	writer.Key("Rotation");
	writer.StartArray();
	writer.Double(static_cast<double>(m_Rotation.x));
	writer.Double(static_cast<double>(m_Rotation.y));
	writer.Double(static_cast<double>(m_Rotation.z));
	writer.EndArray();

	writer.Key("Scale");
	writer.StartArray();
	writer.Double(static_cast<double>(m_Scale.x));
	writer.Double(static_cast<double>(m_Scale.y));
	writer.Double(static_cast<double>(m_Scale.z));
	writer.EndArray();
}

void TransformComponent::Deserialize(const rapidjson::Value& value)
{
	m_Position = FVector3{ 
		static_cast<float>(value["Position"].GetArray()[0].GetDouble()),
		static_cast<float>(value["Position"].GetArray()[1].GetDouble()) ,
		static_cast<float>(value["Position"].GetArray()[2].GetDouble()) 
	};

	m_Rotation = FVector3{
		static_cast<float>(value["Rotation"].GetArray()[0].GetDouble()),
		static_cast<float>(value["Rotation"].GetArray()[1].GetDouble()) ,
		static_cast<float>(value["Rotation"].GetArray()[2].GetDouble())
	};

	m_Scale = FVector3{
		static_cast<float>(value["Scale"].GetArray()[0].GetDouble()),
		static_cast<float>(value["Scale"].GetArray()[1].GetDouble()) ,
		static_cast<float>(value["Scale"].GetArray()[2].GetDouble())
	};
}

FMatrix4 TransformComponent::GetWorldMatrix() const
{
	FVector3 rotation{GetRotation()};

	return MakeTranslation(GetPosition()) * FMatrix4{ MakeRotationZYX(rotation.x, rotation.y, rotation.z) };//*FMatrix4{ MakeScale(m_Scale.x, m_Scale.y, m_Scale.z) };
}

FVector3 TransformComponent::GetPosition() const
{
	if(m_pGameobject->GetParent() == nullptr) return m_Position;

	return m_Position + m_pGameobject->GetParent()->GetTransform()->GetPosition();
}

void TransformComponent::SetPosition(FVector3 position)
{
	m_Position = position;
}

FVector3 TransformComponent::GetRotation() const
{
	if (m_pGameobject->GetParent() == nullptr) return m_Rotation;

	return m_Rotation + m_pGameobject->GetParent()->GetTransform()->GetRotation();
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

void MeshComponent::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("MeshPath");
	writer.String(m_pMesh->GetFilename().c_str());
	writer.Key("SubmeshId");
	writer.Int(m_pMesh->GetSubmeshID());
}

void MeshComponent::Deserialize(const rapidjson::Value& value)
{
	m_pMesh = ResourceManager::GetInstance()->GetMesh(value["MeshPath"].GetString())->GetSubMesh(value["SubmeshId"].GetInt());
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

void RigidBodyComponent::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>&)
{
}

void RigidBodyComponent::Deserialize(const rapidjson::Value&)
{
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

void Rotator::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("Enabled");
	writer.Bool(m_Enabled);

	writer.Key("RotationSpeed");
	writer.Double(static_cast<double>(m_RotationSpeed));

	writer.Key("Axis");
	writer.StartArray();
	writer.Double(static_cast<double>(m_Axis.x));
	writer.Double(static_cast<double>(m_Axis.y));
	writer.Double(static_cast<double>(m_Axis.z));
	writer.EndArray();
}

void Rotator::Deserialize(const rapidjson::Value& value)
{
	m_Enabled = value["Enabled"].GetBool();
	m_RotationSpeed = static_cast<float>(value["RotationSpeed"].GetDouble());

	m_Axis = FVector3{
		static_cast<float>(value["Axis"].GetArray()[0].GetDouble()),
		static_cast<float>(value["Axis"].GetArray()[1].GetDouble()) ,
		static_cast<float>(value["Axis"].GetArray()[2].GetDouble())
	};

}
