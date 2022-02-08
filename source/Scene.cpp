#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"

Scene::Scene()
{
}

void Scene::AddGameObject(GameObject* pGameObject)
{
	m_pGameObjects.push_back(pGameObject);
}

void Scene::Render(Camera* pCamera)
{
	for (auto iter = m_pGameObjects.begin(); iter != m_pGameObjects.end(); iter++)
	{
		(*iter)->Render(pCamera);
	}
}

void Scene::Update(float dt)
{
	for (auto iter = m_pGameObjects.begin(); iter != m_pGameObjects.end(); iter++)
	{
		(*iter)->Update(dt);
	}
}

Scene::~Scene()
{
	for (auto iter = m_pGameObjects.begin(); iter < m_pGameObjects.end(); iter++)
		delete* iter;

	m_pGameObjects.clear();
}
