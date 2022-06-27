#include "GameObject.h"
#include "Component.h"
#include "Scene.h"

#include <imgui.h>

uint32_t GameObject::m_AmountOfGameObjects{};

GameObject::GameObject(const std::string& name, FVector3 position, FVector3 rotation, FVector3 scale)
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
		(*iter)->Update(dt, this);

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
	return m_pScene;
}

void GameObject::AddChild(GameObject* child)
{
	m_pChildren.push_back(child);
}

void GameObject::RemoveChild(GameObject* child)
{
	m_pChildren.erase(std::remove(m_pChildren.begin(), m_pChildren.end(), child));
}
