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

Scene::~Scene()
{
	for (auto iter = m_pGameObjects.begin(); iter < m_pGameObjects.end(); ++iter)
		delete* iter;

	for (auto iter = m_pMaterials.begin(); iter != m_pMaterials.end(); ++iter)
		delete iter->second;

	m_pMaterials.clear();
	m_pGameObjects.clear();
}


void Scene::AddMaterial(const std::string& name, Material* pMaterial)
{
	if (m_pMaterials[name] != nullptr)
	{
		delete pMaterial;
		return;
	}

	m_pMaterials[name] = pMaterial;
}

Material* Scene::GetMaterial(const std::string& name)
{
	return m_pMaterials[name];
}

Material* Scene::GetLatestMaterial()
{
	return m_pMaterials.rbegin()->second;
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
