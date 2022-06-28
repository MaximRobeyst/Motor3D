#pragma once
#include <map>
#include <string>

class Material;

class MaterialManager
{
public:
	~MaterialManager();

	static MaterialManager* GetInstance();

	void AddMaterial(const std::string& name, Material* pMaterial);
	Material* GetMaterial(const std::string& name);
	Material* GetLatestMaterial();

private:
	MaterialManager() = default;

	static MaterialManager* m_pMaterialManager;

	std::map<std::string, Material*> m_pMaterials{};
};

