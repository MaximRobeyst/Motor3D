#include "MaterialManager.h"
#include "Material.h"

MaterialManager* MaterialManager::m_pMaterialManager;

MaterialManager::~MaterialManager()
{
    for (auto iter = m_pMaterials.begin(); iter != m_pMaterials.end(); ++iter)
        delete iter->second;

    m_pMaterials.clear();
}

MaterialManager* MaterialManager::GetInstance()
{
    if (m_pMaterialManager == nullptr) m_pMaterialManager = new MaterialManager();

    return m_pMaterialManager;
}

void MaterialManager::AddMaterial(const std::string& name, Material* pMaterial)
{
    if (pMaterial == nullptr) return;
    if (m_pMaterials[name] != nullptr)
    {
        delete pMaterial;
        return;
    }

    m_pMaterials[name] = pMaterial;
}

Material* MaterialManager::GetMaterial(const std::string& name)
{
    if (m_pMaterials[name] != nullptr && m_pMaterials[name]->GetName() != name) m_pMaterials[name]->SetName(name);
    return m_pMaterials[name];
}

Material* MaterialManager::GetLatestMaterial()
{
    return m_pMaterials.rbegin()->second;
}

void MaterialManager::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.Key("Materials");
    writer.StartArray();

    for (auto& material : m_pMaterials)
    {
        writer.StartObject();

        writer.Key("Type");
        writer.String(typeid(*material.second).name());

        writer.Key("Name");
        writer.String(material.second->GetName().c_str());

        material.second->Serialize(writer);

        writer.EndObject();
    }

    writer.EndArray();
}

void MaterialManager::Deserialize(Scene* /*pScene*/, const rapidjson::Value& value)
{
    for (auto iter = m_pMaterials.begin(); iter != m_pMaterials.end(); ++iter)
        delete iter->second;

    m_pMaterials.clear();

    for (auto& mat : value["Materials"].GetArray())
    {
        // Factory create material type
        auto name = mat["Name"].GetString();

        AddMaterial(name, Material::Deserialize(nullptr, mat));
    }
}
