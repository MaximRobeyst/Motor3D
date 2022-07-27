#include "GameObject.h"
#include "Component.h"
#include "Scene.h"

#include "Factory.h"

#include <imgui.h>
#include <ImGuizmo.h>

#include "TransformComponent.h"


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

	if(m_Started)
		component->Start();
}

void GameObject::RemoveComponent(IComponent* component)
{
	m_pComponents.erase(std::remove(m_pComponents.begin(), m_pComponents.end(), component));
}

void GameObject::Start()
{
	if (!m_Enabled) return;

	for (auto iter = m_pComponents.begin(); iter != m_pComponents.end(); ++iter)
		(*iter)->Start();

	for (auto iter = m_pChildren.begin(); iter != m_pChildren.end(); ++iter)
		(*iter)->Start();
	m_Started = true;
}

void GameObject::Render(Camera* pCamera)
{
	if (!m_Enabled) return;

	for (auto iter = m_pComponents.begin(); iter != m_pComponents.end(); ++iter)
		(*iter)->Render();

	for (auto iter = m_pChildren.begin(); iter != m_pChildren.end(); ++iter)
		(*iter)->Render(pCamera);
}

void GameObject::Update(float dt)
{
	if (!m_Enabled) return;

	for (auto iter = m_pComponents.begin(); iter != m_pComponents.end(); ++iter)
		(*iter)->Update(dt);

	for (auto iter = m_pChildren.begin(); iter != m_pChildren.end(); ++iter)
		(*iter)->Update(dt);
}

void GameObject::RenderGUI()
{
	char chars[128];
	strcpy_s(chars, m_Name.c_str());
	if (ImGui::InputText("Name: ", chars, 128))
	{
		m_Name = chars;
	}
	ImGui::SameLine();
	ImGui::Checkbox("Enabled", &m_Enabled);

	static std::vector<bool> components( true );
	if (components.size() != m_pComponents.size())
	{
		components.resize(m_pComponents.size());
		std::fill(components.begin(), components.end(), true);
	}

	for (int i = 0; i < m_pComponents.size(); ++i)
	{
		bool b = components[i];
		if (ImGui::CollapsingHeader(typeid(*m_pComponents[i]).name(), &b))
		{
			m_pComponents[i]->RenderGUI();
		}
		//if (b)
		//	RemoveComponent(m_pComponents[i]);

		components[i] = b;
	}

	//for (int i = 0 ; i < components.size(); ++i)
	//{
	//	if (components[i])
	//	{
	//		RemoveComponent(m_pComponents[i]);
	//		components.erase(std::remove(components.begin(), components.end(), components[i]));
	//	}
	//}
	

	if (ImGui::BeginPopupContextItem("New Component"))
	{
		auto names = Factory<IComponent>::GetInstance().GetComponentNames();

		static ImGuiTextFilter filter;
		filter.Draw();
		for (size_t i = 0; i < names.size(); ++i)
			if (filter.PassFilter(names[i].c_str()))
			{
				if (ImGui::Button(names[i].c_str()))
				{
					AddComponent(Factory<IComponent>::GetInstance().Create(names[i]));
				}
			}

		ImGui::EndPopup();
	}

	if (ImGui::Button("Add Component", ImVec2{ ImGui::GetWindowWidth(), 25}))
	{
		ImGui::OpenPopup("New Component");
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

void GameObject::SetEnabled(bool value)
{
	m_Enabled = value;
}

bool GameObject::GetEnabled() const
{
	return m_Enabled;
}

void GameObject::AddChild(GameObject* child)
{
	m_pChildren.push_back(child);
}

void GameObject::RemoveChild(GameObject* child)
{
	m_pChildren.erase(std::remove(m_pChildren.begin(), m_pChildren.end(), child));
}
