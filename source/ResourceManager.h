#pragma once
#include <map>
#include <string>

class Mesh;

class ResourceManager
{
public:
	static ResourceManager* GetInstance();

	void AddMesh(const std::string& name, Mesh* pMesh);
	Mesh* GetMesh(const std::string& name);
	

private:
	ResourceManager() = default;
	static ResourceManager* m_pResourceManager;

	std::map<std::string, Mesh*> m_pMeshes{};
};

