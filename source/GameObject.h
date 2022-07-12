#pragma once

#pragma warning (push, 0)
#include "DataTypes.h"
#pragma warning(pop)

#include <vector>
#include <memory>
#include <stringbuffer.h>
#include <prettywriter.h>

#undef max
#undef min
#include <document.h>

#include <string>
#include <DirectXMath.h>

class TransformComponent;
class IComponent;
class Camera;
class Scene;

class GameObject
{
public:
	GameObject(const std::string& name = "new Entity", DirectX::XMFLOAT3 position = DirectX::XMFLOAT3{}, DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3{}, DirectX::XMFLOAT3 scale = {1,1,1});
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

	void Start();
	void Render(Camera* pCamera);
	void Update(float dt);

	void RenderGUI();

	std::string GetName() const;
	TransformComponent* GetTransform();

	GameObject* GetParent() const;
	void SetParent(GameObject* parent);

	int GetChildCount() const;
	GameObject* GetChild(int index) const;

	void SetScene(Scene* pScene);
	Scene* GetScene() const;

	void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>& writer);
	static GameObject* Deserialize(Scene* pScene, const rapidjson::Value& value);

	void SetEnabled(bool value);
	bool GetEnabled() const;

private:
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);

	TransformComponent* m_pTransform;

	uint32_t m_Id;
	static uint32_t m_AmountOfGameObjects;

	std::vector<IComponent*> m_pComponents;

	Scene* m_pScene;
	std::string m_Name;
	bool m_Enabled{ true };
	bool m_Started{ false };

	GameObject* m_pParent;
	std::vector<GameObject*> m_pChildren;
};