#pragma once
#include <map>
#include <string>
#include <vector>

class Mesh;
class Texture;

class ResourceManager
{
public:
	static ResourceManager* GetInstance();

	void AddMesh(const std::string& name, Mesh* pMesh);
	Mesh* GetMesh(const std::string& name);
	void AddTexture(const std::string& name, Texture* pTexture);
	Texture* GetTexture(const std::string& name);

	
	void AddMeshFile(const std::string& filename);
	void AddTextureFile(const std::string& filename);

	std::vector<std::string> GetMeshNames(bool includeFileNames = true) const;
	std::vector<std::string> GetTextureNames(bool includeFileNames = true) const;

private:
	ResourceManager() = default;
	static ResourceManager* m_pResourceManager;

	std::map<std::string, Mesh*> m_pMeshes{};
	std::map<std::string, Texture*> m_pTextures{};

	std::vector<std::string> m_MeshFiles{};
	std::vector<std::string> m_TextureFiles{};
};

