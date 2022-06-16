#pragma once

#pragma warning (push, 0)
#include "DataTypes.h"
#pragma warning(pop)

#include <vector>
#include <memory>

// i know this isn't the best architecture but i wanted a feasable project and programming something like ECS
// in a week is a bit out of scope seeing as i have never done that before so for now i will use this primitive implementation

class TransformComponent;
class IComponent;
class Camera;

class GameObject
{
public:
	GameObject(const std::string& name = "new Entity", FVector3 position = FVector3{}, FVector3 rotation = FVector3{}, FVector3 scale = {});
	GameObject(TransformComponent* pTransformComponent, const std::string& name = "New Entity");
	~GameObject();

	void AddComponent(IComponent* component);
	void RemoveComponent(IComponent* component);

	template <typename T>
	T* GetComponent() const
	{
		// This can be done way better
		for (auto& comp : m_pComponents)
		{
			T* component = dynamic_cast<T*>(comp);
			if (component != nullptr)
				return component;
		}

		return nullptr;
	}

	void Render(Camera* pCamera);
	void Update(float dt);

	void RenderGUI();

	std::string GetName() const;
	TransformComponent* GetTransform();

	GameObject* GetParent() const;
	void SetParent(GameObject* parent);

	int GetChildCount() const;
	GameObject* GetChild(int index) const;

private:
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);

	TransformComponent* m_pTransform;

	uint32_t m_Id;
	static uint32_t m_AmountOfGameObjects;

	std::vector<IComponent*> m_pComponents;
	std::string m_Name;

	GameObject* m_pParent;
	std::vector<GameObject*> m_pChildren;
};