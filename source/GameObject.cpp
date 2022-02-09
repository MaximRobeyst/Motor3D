#include "GameObject.h"
#include "Component.h"

uint32_t GameObject::m_AmountOfGameObjects{};

GameObject::GameObject(const std::string& name, FVector3 position, FVector3 rotation, FVector3 scale)
	: GameObject(new TransformComponent(position, rotation, scale))
{
}

GameObject::GameObject(TransformComponent* pTransformComponent, const std::string& name)
	: m_Id{m_AmountOfGameObjects}
	, m_Name{name}
{
	m_pComponents.push_back(pTransformComponent);
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
	m_pComponents.push_back(component);
}

void GameObject::RemoveComponent(IComponent* component)
{

}

void GameObject::Render(Camera* pCamera)
{
	for (auto iter = m_pComponents.begin(); iter != m_pComponents.end(); ++iter)
		(*iter)->Render(pCamera, this);
}

void GameObject::Update(float dt)
{
	for (auto iter = m_pComponents.begin(); iter != m_pComponents.end(); ++iter)
		(*iter)->Update(dt, this);
}
