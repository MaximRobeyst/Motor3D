#include "TransformComponent.h"
#include "Factory.h"
#include "Utils.h"

#include <imgui.h>
#include <DirectXMath.h>

const Creator<IComponent, TransformComponent> g_TransformCreator{};

TransformComponent::TransformComponent(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
	: IComponent{}
	, m_Position{ pos }
	, m_Rotation{ rotation }
	, m_Scale{ scale }
{
}

void TransformComponent::Start()
{
	GetWorldMatrix();
	UpdateDirections();

}

void TransformComponent::Update()
{
	GetWorldMatrix();
	UpdateDirections();
}

void TransformComponent::RegisterMembers()
{
	ClassMeta<TransformComponent>::AddMemberPtr("Position", &TransformComponent::m_Position);
	ClassMeta<TransformComponent>::AddMemberPtr("Rotation", &TransformComponent::m_Rotation);
	ClassMeta<TransformComponent>::AddMemberPtr("Scale", &TransformComponent::m_Scale);
}

void TransformComponent::RenderGUI()
{
	ClassMeta<TransformComponent>::RenderGUI<TransformComponent>(*this);

	//bool changes{ false };
	//
	////ImGui::InputFloat3("Forward", &m_Forward.x);
	////ImGui::InputFloat3("Right", &m_Right.x);
	////ImGui::InputFloat3("Up", &m_Up.x);
	//
	//float position[] = { m_Position.x, m_Position.y, m_Position.z };
	//if (ImGui::InputFloat3("Position", position))
	//{
	//	m_Position.x = position[0];
	//	m_Position.y = position[1];
	//	m_Position.z = position[2];
	//	changes = true;
	//}
	//
	//float rotation[] = { m_Rotation.x * static_cast<float>(TO_REGREES), m_Rotation.y * static_cast<float>(TO_REGREES), m_Rotation.z * static_cast<float>(TO_REGREES) };
	//if (ImGui::InputFloat3("Rotation", rotation))
	//{
	//	m_Rotation.x = rotation[0] * static_cast<float>(TO_RADIANS);
	//	m_Rotation.y = rotation[1] * static_cast<float>(TO_RADIANS);
	//	m_Rotation.z = rotation[2] * static_cast<float>(TO_RADIANS);
	//	changes = true;
	//}
	//
	//float scale[] = { m_Scale.x, m_Scale.y, m_Scale.z };
	//if (ImGui::InputFloat3("Scale", scale))
	//{
	//	m_Scale.x = scale[0];
	//	m_Scale.y = scale[1];
	//	m_Scale.z = scale[2];
	//	changes = true;
	//}
	//
	//if (changes)
	//{
	//	GetWorldMatrix();
	//	UpdateDirections();
	//}

}

void TransformComponent::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{

	ClassMeta<TransformComponent>::Serialize<TransformComponent>(*this, writer);

	//writer.Key("Position");
	//writer.StartArray();
	//writer.Double(static_cast<double>(m_Position.x));
	//writer.Double(static_cast<double>(m_Position.y));
	//writer.Double(static_cast<double>(m_Position.z));
	//writer.EndArray();
	//
	//writer.Key("Rotation");
	//writer.StartArray();
	//writer.Double(static_cast<double>(m_Rotation.x));
	//writer.Double(static_cast<double>(m_Rotation.y));
	//writer.Double(static_cast<double>(m_Rotation.z));
	//writer.EndArray();
	//
	//writer.Key("Scale");
	//writer.StartArray();
	//writer.Double(static_cast<double>(m_Scale.x));
	//writer.Double(static_cast<double>(m_Scale.y));
	//writer.Double(static_cast<double>(m_Scale.z));
	//writer.EndArray();
}

void TransformComponent::Deserialize(const rapidjson::Value& value)
{
	m_pGameobject->RemoveComponent(m_pGameobject->GetComponent<TransformComponent>());
	ClassMeta<TransformComponent>::Deserialize(*this, value);

	//m_Position = DirectX::XMFLOAT3{
	//	static_cast<float>(value["Position"].GetArray()[0].GetDouble()),
	//	static_cast<float>(value["Position"].GetArray()[1].GetDouble()) ,
	//	static_cast<float>(value["Position"].GetArray()[2].GetDouble())
	//};
	//
	//m_Rotation = DirectX::XMFLOAT3{
	//	static_cast<float>(value["Rotation"].GetArray()[0].GetDouble()),
	//	static_cast<float>(value["Rotation"].GetArray()[1].GetDouble()) ,
	//	static_cast<float>(value["Rotation"].GetArray()[2].GetDouble())
	//};
	//
	//m_Scale = DirectX::XMFLOAT3{
	//	static_cast<float>(value["Scale"].GetArray()[0].GetDouble()),
	//	static_cast<float>(value["Scale"].GetArray()[1].GetDouble()) ,
	//	static_cast<float>(value["Scale"].GetArray()[2].GetDouble())
	//};
}

DirectX::XMFLOAT4X4 TransformComponent::GetWorldMatrix()
{
	if (!m_Dirty) return m_WorldMatrix;

	auto rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(m_Rotation.x * static_cast<float>(TO_RADIANS), m_Rotation.y * static_cast<float>(TO_RADIANS), m_Rotation.z * static_cast<float>(TO_RADIANS));
	auto worldMatrix = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) * DirectX::XMMatrixRotationQuaternion(rotationQuaternion) * DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

	if (m_pGameobject->GetParent() != nullptr)
	{
		const auto parentWorld = m_pGameobject->GetParent()->GetTransform()->GetWorldMatrix();
		worldMatrix *= DirectX::XMLoadFloat4x4(&parentWorld);
	}

	DirectX::XMFLOAT4X4 matrixToReturn;

	DirectX::XMStoreFloat4x4(&matrixToReturn, worldMatrix);
	m_WorldMatrix = matrixToReturn;

	return matrixToReturn;
}

DirectX::XMFLOAT3 TransformComponent::GetPosition() const
{
	if (m_pGameobject->GetParent() == nullptr) return m_Position;

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
	m_Dirty = true;
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
	m_Dirty = true;
}

DirectX::XMFLOAT3 TransformComponent::GetScale() const
{
	return m_Scale;
}

void TransformComponent::SetScale(DirectX::XMFLOAT3 scale)
{
	m_Scale = scale;
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

void TransformComponent::UpdateDirections()
{
	if (!m_Dirty) return;

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