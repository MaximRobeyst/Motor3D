#include "Component.h"
#include "Mesh.h"
#include "GameObject.h"

#include <imgui.h>
#include "Utils.h"
#include "Factory.h"
#include "ResourceManager.h"
#include "Scene.h"

const Creator<IComponent, TransformComponent> g_TransformCreator{};
const Creator<IComponent, RigidBodyComponent> g_RigidbodyCreator{};
const Creator<IComponent, MeshComponent> g_MeshComponent{};
const Creator<IComponent, Rotator> g_RotatorComponent{};
const Creator<IComponent, CameraComponent> g_CameraComponent{};

TransformComponent::TransformComponent(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
	: IComponent{0}
	, m_Position{pos}
	, m_Rotation{rotation}
	, m_Scale{scale}
{
}

void TransformComponent::Update(float /*dt*/)
{
	GetWorldMatrix();

	auto worldMatrix = DirectX::XMLoadFloat4x4(&m_WorldMatrix);

	DirectX::XMVECTOR pos, rot, scale; 
	DirectX::XMMatrixDecompose(&scale, &rot, &pos, worldMatrix);

	auto rotMat = DirectX::XMMatrixRotationQuaternion(rot);
	const auto forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0, 0, 1, 0), rotMat);
	const auto right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1, 0, 0, 0), rotMat);
	const auto up = DirectX::XMVector3Cross(forward, right);

	DirectX::XMStoreFloat3(&m_Forward, forward);
	DirectX::XMStoreFloat3(&m_Right, right);
	DirectX::XMStoreFloat3(&m_Up, up);
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
	m_Position = DirectX::XMFLOAT3{
		static_cast<float>(value["Position"].GetArray()[0].GetDouble()),
		static_cast<float>(value["Position"].GetArray()[1].GetDouble()) ,
		static_cast<float>(value["Position"].GetArray()[2].GetDouble()) 
	};

	m_Rotation = DirectX::XMFLOAT3{
		static_cast<float>(value["Rotation"].GetArray()[0].GetDouble()),
		static_cast<float>(value["Rotation"].GetArray()[1].GetDouble()) ,
		static_cast<float>(value["Rotation"].GetArray()[2].GetDouble())
	};

	m_Scale = DirectX::XMFLOAT3{
		static_cast<float>(value["Scale"].GetArray()[0].GetDouble()),
		static_cast<float>(value["Scale"].GetArray()[1].GetDouble()) ,
		static_cast<float>(value["Scale"].GetArray()[2].GetDouble())
	};
}

DirectX::XMFLOAT4X4 TransformComponent::GetWorldMatrix()
{
	DirectX::XMFLOAT3 rotation{GetRotation()};
	DirectX::XMFLOAT3 position = GetPosition();

	auto translationMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	auto rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	auto worldMatrix = translationMatrix * rotationMatrix;
	DirectX::XMFLOAT4X4 matrixToReturn;

	DirectX::XMStoreFloat4x4(&matrixToReturn, worldMatrix);
	m_WorldMatrix = matrixToReturn;

	return matrixToReturn;
}

DirectX::XMFLOAT3 TransformComponent::GetPosition() const
{
	if(m_pGameobject->GetParent() == nullptr) return m_Position;

	auto position = DirectX::XMLoadFloat3(&m_Position);
	auto parentpos = m_pGameobject->GetParent()->GetTransform()->GetPosition();
	auto parentPosition = DirectX::XMLoadFloat3(&parentpos);

	DirectX::XMFLOAT3 positionToReturn;
	DirectX::XMStoreFloat3(&positionToReturn, DirectX::XMVectorAdd(position, parentPosition));

	return positionToReturn;
}

void TransformComponent::SetPosition(DirectX::XMFLOAT3 position)
{
	m_Position = position;
}

DirectX::XMFLOAT3 TransformComponent::GetRotation() const
{
	if (m_pGameobject->GetParent() == nullptr) return m_Rotation;

	auto rotation = DirectX::XMLoadFloat3(&m_Rotation);
	auto parentrot = m_pGameobject->GetParent()->GetTransform()->GetRotation();
	auto parentRotation = DirectX::XMLoadFloat3(&parentrot);

	DirectX::XMFLOAT3 rotationToReturn;
	DirectX::XMStoreFloat3(&rotationToReturn, DirectX::XMVectorAdd(rotation, parentRotation));

	return rotationToReturn;
}

void TransformComponent::SetRotation(DirectX::XMFLOAT3 rotation)
{
	m_Rotation = rotation;
}

void TransformComponent::SetParent(TransformComponent* /*pTransformComponent*/)
{
}

DirectX::XMFLOAT3& TransformComponent::GetForward()
{
	return m_Forward;
}

DirectX::XMFLOAT3& TransformComponent::GetRight()
{
	return m_Right;
}

DirectX::XMFLOAT3& TransformComponent::GetUp()
{
	return m_Up;
}

IComponent::IComponent(uint8_t componentID)
	: m_ComponentID{componentID}
{
}

void IComponent::Render(Camera* /*pCamera*/, GameObject* /*pGameobject*/)
{
}

void IComponent::Update(float /*dt*/)
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

void MeshComponent::Render(Camera* , GameObject* /*pGameobject*/)
{
	m_pMesh->Render(MyEngine::GetSingleton()->GetDeviceContext(), m_pGameobject->GetScene()->GetCamera());
}

void MeshComponent::Update(float)
{
	m_pMesh->SetWorldMatrix(m_pGameobject->GetComponent<TransformComponent>()->GetWorldMatrix());
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

RigidBodyComponent::RigidBodyComponent(DirectX::XMFLOAT3 velocity, DirectX::XMFLOAT3 acceleration, DirectX::XMFLOAT3 gravity)
	: IComponent(0)
	, m_Velocity{velocity}
	, m_Acceleration{acceleration}
	, m_Gravity{gravity}
{
}

void RigidBodyComponent::Update(float)
{
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

Rotator::Rotator(float rotationSpeed, DirectX::XMFLOAT3 axis)
	: IComponent(4)
	, m_Rotation{}
	, m_RotationSpeed{rotationSpeed}
	, m_Axis{axis}
{
}

void Rotator::Update(float dt)
{
	if (!m_Enabled) return;

	m_Rotation += (ToRadians(m_RotationSpeed) * dt);

	auto axis = DirectX::XMLoadFloat3(&m_Axis);
	auto rot = DirectX::XMLoadFloat(&m_Rotation);

	DirectX::XMFLOAT3 rotation;
	DirectX::XMStoreFloat3(&rotation, DirectX::XMVectorMultiply(axis , rot));

	m_pGameobject->GetTransform()->SetRotation(rotation);
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

	m_Axis = DirectX::XMFLOAT3{
		static_cast<float>(value["Axis"].GetArray()[0].GetDouble()),
		static_cast<float>(value["Axis"].GetArray()[1].GetDouble()) ,
		static_cast<float>(value["Axis"].GetArray()[2].GetDouble())
	};

}

CameraComponent::CameraComponent(float /*FOV*/, float aspectRatio, float cfar, float cnear)
	: IComponent{0}
	, m_FOV{ DirectX::XM_PIDIV4 }
	, m_AspectRatio{aspectRatio}
	, m_Far{cfar}
	, m_Near{cnear}
{
}

void CameraComponent::Start()
{
	m_pTransformComponent = m_pGameobject->GetTransform();
	m_pGameobject->GetScene()->SetCamera(this);
}

void CameraComponent::Update(float /*elapsedSec*/)
{
	DirectX::XMMATRIX projection{};
	projection = DirectX::XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, m_Near, m_Far);

	auto position = m_pGameobject->GetTransform()->GetPosition();
	auto forward = m_pGameobject->GetTransform()->GetForward();
	auto up = m_pGameobject->GetTransform()->GetUp();

	const DirectX::XMVECTOR worldPosition = DirectX::XMLoadFloat3(&position);
	const DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&forward);
	const DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

	auto LookAt = DirectX::XMVectorAdd(worldPosition, lookAt);

	const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(worldPosition, LookAt, upVec);
	const DirectX::XMMATRIX viewInv = DirectX::XMMatrixInverse(nullptr, view);
	const DirectX::XMMATRIX viewProjectionInv = DirectX::XMMatrixInverse(nullptr, view * projection);

	DirectX::XMStoreFloat4x4(&m_ProjectionMatrix, projection);
	DirectX::XMStoreFloat4x4(&m_View, view);
	DirectX::XMStoreFloat4x4(&m_ViewInv, viewInv);
	DirectX::XMStoreFloat4x4(&m_ViewProjection, view * projection);
	DirectX::XMStoreFloat4x4(&m_ViewProjectionInv, viewProjectionInv);

}
