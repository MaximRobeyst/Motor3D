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
        delete m_pMaterials[name];
    }

    m_pMaterials[name] = pMaterial;
}

Material* MaterialManager::GetMaterial(const std::string& name)
{
    return m_pMaterials[name];
}

Material* MaterialManager::GetLatestMaterial()
{
    return m_pMaterials.rbegin()->second;
}
