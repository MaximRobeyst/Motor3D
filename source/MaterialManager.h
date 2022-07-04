#pragma once
#include <map>
#include <string>

#include <stringbuffer.h>
#include <prettywriter.h>
#undef max
#undef min
#include <document.h>

class Material;
class Scene;

class MaterialManager
{
public:
	~MaterialManager();

	static MaterialManager* GetInstance();

	void AddMaterial(const std::string& name, Material* pMaterial);
	Material* GetMaterial(const std::string& name);
	Material* GetLatestMaterial();

	void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>& writer);
	void Deserialize(Scene* pScene, const rapidjson::Value& value);

private:
	MaterialManager() = default;

	static MaterialManager* m_pMaterialManager;

	std::map<std::string, Material*> m_pMaterials{};
};

