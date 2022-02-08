#pragma once
#include <string>
#include <vector>

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

	void Render(Camera* pCamera);
	void Update(float dt);

private:
	std::vector<GameObject*> m_pGameObjects{};
};

