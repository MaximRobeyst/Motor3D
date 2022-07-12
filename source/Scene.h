#pragma once
#include <string>
#include <vector>
#include "Material.h"

typedef int ImGuiTreeNodeFlags;

// The scene keeps track of all the components and entities it will also be able to load in save files and save to files
class GameObject;
class Camera;
class CameraComponent;
class Scene
{
public:
	Scene();
	~Scene();

	void AddGameObject(GameObject* pGameObject);
	void RemoveGameobject(GameObject* pGameobject);

	void AddMaterial(const std::string& name, Material* pMaterial);
	Material* GetMaterial(const std::string& name);
	Material* GetLatestMaterial();
	
	void Start();
	void Render(Camera* pCamera);
	void RenderGUI();

	void Update(float dt);

	void Serialize(const std::string& filename);
	void Deserialize(const std::string& filename);

	void SetCamera(CameraComponent* pCameraComponent);
	Camera* GetCamera() const;
	GameObject* GetSelectedObject() const;

private:
	void RenderGameobjectSceneGraph(GameObject* pGameobject,int i, ImGuiTreeNodeFlags node_flags, int& node_clicked, bool test_drag_and_drop);

	std::vector<GameObject*> m_pGameObjects{};
	std::map<std::string, Material*> m_pMaterials{};

	GameObject* m_pSelectedGameobject{};
	CameraComponent* m_pCameraComponent;
};

