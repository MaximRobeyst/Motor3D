#pragma once
#include <string>
#include <vector>
#include "Material.h"

typedef int ImGuiTreeNodeFlags;

// The scene keeps track of all the components and entities it will also be able to load in save files and save to files
class GameObject;
class Camera;
class Scene
{
public:
	Scene();
	Scene(std::string fileName); // this is the function that will load in the scene save file

	~Scene();

	void AddGameObject(GameObject* pGameObject);
	void RemoveGameobject(GameObject* pGameobject);

	void AddMaterial(const std::string& name, Material* pMaterial);
	Material* GetMaterial(const std::string& name);
	Material* GetLatestMaterial();

	void Render(Camera* pCamera);
	void RenderGUI();

	void Update(float dt);

private:
	void RenderGameobjectSceneGraph(GameObject* pGameobject,int i, ImGuiTreeNodeFlags node_flags, int& node_clicked, bool test_drag_and_drop);


	std::vector<GameObject*> m_pGameObjects{};
	std::map<std::string, Material*> m_pMaterials{};

	GameObject* m_pSelectedGameobject{};
};

