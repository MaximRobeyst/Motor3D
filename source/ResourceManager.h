#pragma once
#include <map>
#include <string>
#include <vector>

class Mesh;

class ResourceManager
{
public:
	static ResourceManager* GetInstance();

	void AddMesh(const std::string& name, Mesh* pMesh);
	Mesh* GetMesh(const std::string& name);
	
	void AddMeshFile(const std::string& filename);

	std::vector<std::string> GetMeshNames(bool includeFileNames = true) const;

private:
	ResourceManager() = default;
	static ResourceManager* m_pResourceManager;

	std::map<std::string, Mesh*> m_pMeshes{};
	std::vector<std::string> m_MeshFiles{};
};

