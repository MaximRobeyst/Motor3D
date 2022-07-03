#include "GameObject.h"
#include "Component.h"
#include "Scene.h"

#include "Factory.h"

#include <imgui.h>


uint32_t GameObject::m_AmountOfGameObjects{};

GameObject::GameObject(const std::string& name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
	: GameObject(new TransformComponent(position, rotation, scale), name)
{
}

GameObject::GameObject(TransformComponent* pTransformComponent, const std::string& name)
	: m_Id{m_AmountOfGameObjects}
	, m_Name{name}
{
	AddComponent(pTransformComponent);
	++m_AmountOfGameObjects;
}

GameObject::~GameObject()
{
	m_Id = INT_MAX;
	--m_AmountOfGameObjects;

	for (auto iter = m_pComponents.begin(); iter != m_pComponents.end(); ++iter)
		delete* iter;
	for (auto iter = m_pChildren.begin(); iter != m_pChildren.end(); ++iter)
		delete* iter;

	m_pComponents.clear();
}

void GameObject::AddComponent(IComponent* component)
{
	component->SetGameobject(this);
	m_pComponents.push_back(component);
}

void GameObject::RemoveComponent(IComponent* component)
{
	m_pComponents.erase(std::remove(m_pComponents.begin(), m_pComponents.end(), component));
}

void GameObject::Start()
{
	for (auto iter = m_pComponents.begin(); iter != m_pComponents.end(); ++iter)
		(*iter)->Start();

	for (auto iter = m_pChildren.begin(); iter != m_pChildren.end(); ++iter)
		(*iter)->Start();
}

void GameObject::Render(Camera* pCamera)
{
	for (auto iter = m_pComponents.begin(); iter != m_pComponents.end(); ++iter)
		(*iter)->Render(pCamera, this);

	for (auto iter = m_pChildren.begin(); iter != m_pChildren.end(); ++iter)
		(*iter)->Render(pCamera);
}

void GameObject::Update(float dt)
{
	for (auto iter = m_pComponents.begin(); iter != m_pComponents.end(); ++iter)
		(*iter)->Update(dt);

	for (auto iter = m_pChildren.begin(); iter != m_pChildren.end(); ++iter)
		(*iter)->Update(dt);
}

void GameObject::RenderGUI()
{
	ImGui::Text(m_Name.c_str());

	for (auto component : m_pComponents)
	{
		if (ImGui::CollapsingHeader(typeid(*component).name()))
		{
			component->RenderGUI();
		}
	}
}

std::string GameObject::GetName() const
{
	return m_Name;
}

TransformComponent* GameObject::GetTransform()
{
	if(m_pTransform != nullptr) return m_pTransform;

	m_pTransform = GetComponent<TransformComponent>();
	return m_pTransform;
}

GameObject* GameObject::GetParent() const
{
	return m_pParent;
}

void GameObject::SetParent(GameObject* parent)
{
	if (parent == this) return;
	if (parent == m_pParent) return;

	if (m_pParent != nullptr)
		m_pParent->RemoveChild(this);

	if (m_pParent == nullptr && m_pScene != nullptr)
		m_pScene->RemoveGameobject(parent);

	m_pParent = parent;

	if (m_pParent != nullptr)
		m_pParent->AddChild(this);

	if (m_pParent == nullptr)
		m_pScene->AddGameObject(this);

	GetTransform()->SetParent(m_pParent->GetTransform());
	m_pScene = m_pParent->GetScene();

}

int GameObject::GetChildCount() const
{
	return static_cast<int>( m_pChildren.size());
}

GameObject* GameObject::GetChild(int index) const
{
	return m_pChildren[index];
}

void GameObject::SetScene(Scene* pScene)
{
	m_pScene = pScene;
}

Scene* GameObject::GetScene() const
{
	if (m_pScene == nullptr && m_pParent != nullptr) return m_pParent->GetScene();

	return m_pScene;
}

void GameObject::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("Name");
	writer.String(m_Name.c_str());

	writer.Key("Components");
	writer.StartArray();

	for (auto& pComponent : m_pComponents)
	{
		writer.StartObject();

		writer.Key("Name");
		writer.String(typeid(*pComponent).name());

		pComponent->Serialize(writer);
		
		writer.EndObject();
	}
	writer.EndArray();

	writer.Key("Children");
	writer.StartArray();

	for (auto& pGameobject : m_pChildren)
	{
		writer.StartObject();

		pGameobject->Serialize(writer);

		writer.EndObject();
	}
	writer.EndArray();
}

GameObject* GameObject::Deserialize(Scene* pScene, const rapidjson::Value& value)
{
	GameObject* pGameobject = new GameObject(value["Name"].GetString());

	for (auto& component : value["Components"].GetArray())
	{
		auto pComponent = Factory<IComponent>::GetInstance().Create(component["Name"].GetString());
		pGameobject->AddComponent(pComponent);
		pComponent->Deserialize(component);
	}

	for (auto& gameobject : value["Children"].GetArray())
	{
		auto pChild = GameObject::Deserialize(pScene, gameobject);
		pChild->SetParent(pGameobject);
	}

	return pGameobject;
}

void GameObject::AddChild(GameObject* child)
{
	m_pChildren.push_back(child);
}

void GameObject::RemoveChild(GameObject* child)
{
	m_pChildren.erase(std::remove(m_pChildren.begin(), m_pChildren.end(), child));
}
