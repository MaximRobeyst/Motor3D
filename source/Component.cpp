#include "Component.h"
#include "Mesh.h"
#include "GameObject.h"

#include <imgui.h>
#include "Utils.h"
#include "Factory.h"
#include "ResourceManager.h"
#include "MaterialManager.h"
#include "Scene.h"
#include "Camera.h"

const Creator<IComponent, TransformComponent> g_TransformCreator{};
const Creator<IComponent, RigidBodyComponent> g_RigidbodyCreator{};
const Creator<IComponent, MeshComponent> g_MeshComponent{};
const Creator<IComponent, Rotator> g_RotatorComponent{};
const Creator<IComponent, CameraComponent> g_CameraComponent{};

TransformComponent::TransformComponent(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
	: IComponent{}
	, m_Position{pos}
	, m_Rotation{rotation}
	, m_Scale{scale}
{
}

void TransformComponent::Start()
{
	GetWorldMatrix();
	UpdateDirections();
}

void TransformComponent::Update(float /*dt*/)
{
	GetWorldMatrix();
	UpdateDirections();
}

void TransformComponent::RenderGUI()
{
	bool changes{ false };

	//ImGui::InputFloat3("Forward", &m_Forward.x);
	//ImGui::InputFloat3("Right", &m_Right.x);
	//ImGui::InputFloat3("Up", &m_Up.x);

	float position[] = { m_Position.x, m_Position.y, m_Position.z };
	if (ImGui::InputFloat3("Position", position))
	{
		m_Position.x = position[0];
		m_Position.y = position[1];
		m_Position.z = position[2];
		changes = true;
	}

	float rotation[] = { m_Rotation.x * static_cast<float>( TO_REGREES), m_Rotation.y * static_cast<float>(TO_REGREES), m_Rotation.z * static_cast<float>(TO_REGREES) };
	if (ImGui::InputFloat3("Rotation", rotation))
	{
		m_Rotation.x = rotation[0] * static_cast<float>(TO_RADIANS);
		m_Rotation.y = rotation[1] * static_cast<float>(TO_RADIANS);
		m_Rotation.z = rotation[2] * static_cast<float>(TO_RADIANS);
		changes = true;
	}

	float scale[] = { m_Scale.x, m_Scale.y, m_Scale.z };
	if (ImGui::InputFloat3("Scale", scale))
	{
		m_Scale.x = scale[0];
		m_Scale.y = scale[1];
		m_Scale.z = scale[2];
		changes = true;
	}

	if (changes)
	{
		GetWorldMatrix();
		UpdateDirections();
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
	m_pGameobject->RemoveComponent(m_pGameobject->GetComponent<TransformComponent>());

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
	if (!m_Dirty) return m_WorldMatrix;

	auto rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
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

IComponent::IComponent()
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
	: IComponent{}
	, m_pMesh{pMesh}
{
}

MeshComponent::~MeshComponent()
{
	delete m_pMesh;
}

void MeshComponent::Start()
{
	m_pTransform = m_pGameobject->GetComponent<TransformComponent>();
	if (m_pMesh == nullptr) return;

	m_pMesh->SetWorldMatrix(m_pTransform->GetWorldMatrix());
}

void MeshComponent::Render(Camera* , GameObject* /*pGameobject*/)
{
	if (m_pMesh == nullptr) return;

	m_pMesh->SetWorldMatrix(m_pTransform->GetWorldMatrix());
	m_pMesh->Render(MyEngine::GetSingleton()->GetDeviceContext(), m_pGameobject->GetScene()->GetCamera());
}

void MeshComponent::Update(float)
{
}

void MeshComponent::RenderGUI()
{
	static ImGuiComboFlags flags = 0;
	auto names = ResourceManager::GetInstance()->GetMeshNames();
	static int item_current_idx = 0; // Here we store our selection data as an index.
	const char* combo_preview_value = names[item_current_idx].c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("combo 1", combo_preview_value, flags))
	{
		for (int n = 0; n < names.size(); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(names[n].c_str(), is_selected))
			{
				item_current_idx = n;
				SetMesh(names[n]);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	
	if (m_pMesh == nullptr) return;

	int submeshId = m_pMesh->GetSubmeshID();
	if (ImGui::InputInt("Submesh", &submeshId))
		m_pMesh = ResourceManager::GetInstance()->GetMesh(m_pMesh->GetFilename() + std::to_string(submeshId));
}

void MeshComponent::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("MeshPath");
	writer.String(m_pMesh->GetFilename().c_str());
	writer.Key("SubmeshId");
	writer.Int(m_pMesh->GetSubmeshID());
	writer.Key("Material");
	writer.String(m_pMesh->GetMaterial("")->GetName().c_str());
}

void MeshComponent::Deserialize(const rapidjson::Value& value)
{
	m_pMesh = ResourceManager::GetInstance()->GetMesh(value["MeshPath"].GetString())->GetSubMesh(value["SubmeshId"].GetInt());

	auto p = MaterialManager::GetInstance();
	Material* mat = MaterialManager::GetInstance()->GetMaterial(value["Material"].GetString());
	m_pMesh->SetMaterial(mat->GetName(), mat);
	p = nullptr;
}

void MeshComponent::SetMesh(const std::string& name)
{
	if (m_pMesh == ResourceManager::GetInstance()->GetMesh(name)) return;

	m_pMesh = ResourceManager::GetInstance()->GetMesh(name);
}

RigidBodyComponent::RigidBodyComponent(DirectX::XMFLOAT3 velocity, DirectX::XMFLOAT3 acceleration, DirectX::XMFLOAT3 gravity)
	: IComponent()
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
	: IComponent()
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

	m_pGameobject->GetTransform()->SetRotation(DirectX::XMFLOAT3{ 0, m_Rotation, 0 });
}

void Rotator::RenderGUI()
{
	ImGui::Checkbox("Enabled Rotator", &m_Enabled);
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

CameraComponent::CameraComponent(float FOV, float aspectRatio, float cfar, float cnear)
	: Camera{FOV, aspectRatio, cfar, cnear}
{
}

void CameraComponent::Start()
{
	m_pTransformComponent = m_pGameobject->GetTransform();
	m_pGameobject->GetScene()->SetCamera(this);

	UpdateMatrix();
}

void CameraComponent::Update(float /*elapsedSec*/)
{
	//UpdateMatrix();
}

void CameraComponent::Render(Camera* , GameObject* )
{
	UpdateMatrix();
}

void CameraComponent::RenderGUI()
{
	ImGui::SliderAngle("Field of View", &m_FOV, 30.f);
	if (ImGui::InputFloat("Near", &m_Near))
	{
		UpdateMatrix();
	}
	if (ImGui::InputFloat("Far", &m_Far))
	{
		UpdateMatrix();
	}
}

void CameraComponent::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("FOV");
	writer.Double(static_cast<double>(m_FOV));
	writer.Key("Far");
	writer.Double(static_cast<double>(m_Far));
	writer.Key("Near");
	writer.Double(static_cast<double>(m_Near));
}

void CameraComponent::Deserialize(const rapidjson::Value& value)
{
	m_FOV = static_cast<float>(value["FOV"].GetDouble());
	m_AspectRatio = MyEngine::GetSingleton()->GetWindowWidth() / MyEngine::GetSingleton()->GetWindowHeight();


}

void CameraComponent::UpdateMatrix()
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
