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
	if (std::find(m_MeshFiles.begin(), m_MeshFiles.end(), name) != m_MeshFiles.end())
	{
		m_MeshFiles.erase(std::remove(m_MeshFiles.begin(), m_MeshFiles.end(), name));
		m_pMeshes[name] = new Mesh(MyEngine::GetSingleton()->GetDevice(), MyEngine::GetSingleton()->GetWindowHandle(), name, new Material(MyEngine::GetSingleton()->GetDevice(), "Resources/material_unlit.fx"));
	}

	if (m_pMeshes[name] == nullptr)
	{
		m_pMeshes[name] = new Mesh(MyEngine::GetSingleton()->GetDevice(), MyEngine::GetSingleton()->GetWindowHandle(), name, new Material(MyEngine::GetSingleton()->GetDevice(), "Resources/material_unlit.fx"));
	}

	return m_pMeshes[name];
}

void ResourceManager::AddMeshFile(const std::string& filename)
{
	if (std::find(m_MeshFiles.begin(), m_MeshFiles.end(), filename) != m_MeshFiles.end()) return;

	m_MeshFiles.emplace_back(filename);
}

std::vector<std::string> ResourceManager::GetMeshNames(bool includeFileNames) const
{
	std::vector<std::string> names;

	for (auto mesh : m_pMeshes)
		names.emplace_back(mesh.first);

	if (!includeFileNames) return names;
	for (auto fileName : m_MeshFiles)
		names.emplace_back(fileName);

	return names;
}
