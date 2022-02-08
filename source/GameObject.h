#pragma once
#include "DataTypes.h"
#include <vector>

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
	template <typename T>
	void AddComponent(T component)
	{
		m_pComponents.push_back(component);
	}

	void RemoveComponent(IComponent* component);

	void Render(Camera* pCamera);
	void Update(float dt);

	//template <typename T>
	//T* GetComponent() const
	//{
	//
	//}

private:
	uint32_t m_Id;
	static uint32_t m_AmountOfGameObjects;

	std::vector<IComponent*> m_pComponents;
	std::string m_Name;
};