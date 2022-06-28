#include "ResourceManager.h"
#include "Mesh.h"
#include "Material.h"

ResourceManager* ResourceManager::m_pResourceManager{};

ResourceManager* ResourceManager::GetInstance()
{
	if (m_pResourceManager == nullptr) m_pResourceManager = new ResourceManager();

	return m_pResourceManager;
}

void ResourceManager::AddMesh(const std::string& name, Mesh* pMesh)
{
	if (pMesh == nullptr) return;
	if (m_pMeshes[name] != nullptr)
	{
		delete m_pMeshes[name];
	}

	m_pMeshes[name] = pMesh;
}

Mesh* ResourceManager::GetMesh(const std::string& name)
{
	if (m_pMeshes[name] == nullptr) m_pMeshes[name] =
		new Mesh(MyEngine::GetSingleton()->GetDevice(), MyEngine::GetSingleton()->GetWindowHandle(), name, new Material(MyEngine::GetSingleton()->GetDevice(), L"Resources/material_unlit.fx"));

	return m_pMeshes[name];
}
