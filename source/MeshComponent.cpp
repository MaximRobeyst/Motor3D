#include "MeshComponent.h"
#include "Factory.h"
#include "TransformComponent.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "MaterialManager.h"

#include <imgui.h>

const Creator<IComponent, MeshComponent> g_MeshComponent{};

MeshComponent::MeshComponent(Mesh* pMesh)
	: IComponent{}
	, m_pMesh{ pMesh }
{
}

MeshComponent::~MeshComponent()
{
	//delete m_pMesh;
}

void MeshComponent::Start()
{
	m_pTransform = m_pGameobject->GetComponent<TransformComponent>();
	if (m_pMesh == nullptr) return;

	m_pMesh->SetWorldMatrix(m_pTransform->GetWorldMatrix());
}

void MeshComponent::Render()
{
	if (m_pMesh == nullptr) return;

	m_pMesh->SetWorldMatrix(m_pTransform->GetWorldMatrix());
	m_pMesh->Render(MyEngine::GetSingleton()->GetDeviceContext(), m_pGameobject->GetScene()->GetCamera());
}

void MeshComponent::Update()
{
}

void MeshComponent::RenderGUI()
{
	static ImGuiComboFlags flags = 0;
	auto names = ResourceManager::GetInstance()->GetMeshNames();
	static int item_current_idx = 0; // Here we store our selection data as an index.
	const char* combo_preview_value = names[item_current_idx].c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("combo 1", combo_preview_value, flags))
	{
		for (int n = 0; n < names.size(); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(names[n].c_str(), is_selected))
			{
				item_current_idx = n;
				SetMesh(names[n]);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (m_pMesh == nullptr) return;

	int submeshId = m_pMesh->GetSubmeshID();
	if (ImGui::InputInt("Submesh", &submeshId))
		m_pMesh = ResourceManager::GetInstance()->GetMesh(m_pMesh->GetFilename() + std::to_string(submeshId));
}

void MeshComponent::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("MeshPath");
	writer.String(m_pMesh->GetFilename().c_str());
	writer.Key("SubmeshId");
	writer.Int(m_pMesh->GetSubmeshID());
	writer.Key("Material");
	writer.String(m_pMesh->GetMaterial("")->GetName().c_str());
}

void MeshComponent::Deserialize(const rapidjson::Value& value)
{
	m_pMesh = ResourceManager::GetInstance()->GetMesh(value["MeshPath"].GetString())->GetSubMesh(value["SubmeshId"].GetInt());

	auto p = MaterialManager::GetInstance();
	Material* mat = MaterialManager::GetInstance()->GetMaterial(value["Material"].GetString());
	m_pMesh->SetMaterial(mat->GetName(), mat);
	p = nullptr;
}

void MeshComponent::SetMesh(Mesh* pMesh)
{
	m_pMesh = pMesh;
}

Mesh* MeshComponent::GetMesh() const
{
	return m_pMesh;
}

void MeshComponent::SetMesh(const std::string& name)
{
	if (m_pMesh == ResourceManager::GetInstance()->GetMesh(name)) return;

	m_pMesh = ResourceManager::GetInstance()->GetMesh(name);
}