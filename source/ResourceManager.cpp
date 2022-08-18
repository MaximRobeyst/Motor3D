#include "ResourceManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

ResourceManager* ResourceManager::m_pResourceManager{};

ResourceManager::~ResourceManager()
{
	for (auto meshPair : m_pMeshes)
		delete meshPair.second;

	for (auto texturePair : m_pTextures)
		delete texturePair.second;

	m_pMeshes.clear();
	m_pTextures.clear();
}

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

void ResourceManager::AddTexture(const std::string& name, Texture* pTexture)
{
	if (pTexture == nullptr) return;
	if (m_pTextures[name] != nullptr)
	{
		delete m_pTextures[name];
	}

	m_pTextures[name] = pTexture;
}

Texture* ResourceManager::GetTexture(const std::string& name)
{
	if (std::find(m_TextureFiles.begin(), m_TextureFiles.end(), name) != m_TextureFiles.end())
	{
		m_TextureFiles.erase(std::remove(m_TextureFiles.begin(), m_TextureFiles.end(), name));
		m_pTextures[name] = new Texture(MyEngine::GetSingleton()->GetDevice(), name);
	}

	if (m_pTextures[name] == nullptr)
	{
		m_pTextures[name] = new Texture(MyEngine::GetSingleton()->GetDevice(), name);
	}

	return m_pTextures[name];
}

void ResourceManager::AddMeshFile(const std::string& filename)
{
	if (std::find(m_MeshFiles.begin(), m_MeshFiles.end(), filename) != m_MeshFiles.end()) return;

	m_MeshFiles.emplace_back(filename);
}

void ResourceManager::AddTextureFile(const std::string& filename)
{
	if (std::find(m_TextureFiles.begin(), m_TextureFiles.end(), filename) != m_TextureFiles.end()) return;

	m_TextureFiles.emplace_back(filename);
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

std::vector<std::string> ResourceManager::GetTextureNames(bool includeFileNames) const
{
	std::vector<std::string> names;

	for (auto texture : m_pTextures)
		names.emplace_back(texture.first);

	if (!includeFileNames) return names;
	for (auto fileName : m_TextureFiles)
		names.emplace_back(fileName);

	return names;
}
