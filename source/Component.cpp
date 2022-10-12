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
#include "TransformComponent.h"
#include "GameTime.h"

const Creator<IComponent, Rotator> g_RotatorComponent{};
const Creator<IComponent, CameraComponent> g_CameraComponent{};


IComponent::IComponent()
{
}

IComponent::~IComponent()
{
}

void IComponent::Render()
{
}

void IComponent::Update()
{
}

void IComponent::SetGameobject(GameObject* gameobject)
{
	m_pGameobject = gameobject;
}

GameObject* IComponent::GetGameObject() const
{
	return m_pGameobject;
}

Rotator::Rotator(float rotationSpeed, DirectX::XMFLOAT3 axis)
	: IComponent()
	, m_Rotation{}
	, m_RotationSpeed{rotationSpeed}
	, m_Axis{axis}
{
}

void Rotator::Update()
{
	if (!m_Enabled) return;

	m_Rotation += (m_RotationSpeed) * GameTime::GetInstance().GetElapsed();

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

void CameraComponent::Update()
{
	//UpdateMatrix();
}

void CameraComponent::Render( )
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
