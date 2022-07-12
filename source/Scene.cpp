
#include <fstream>
#include <stringbuffer.h>
#include <prettywriter.h>
#include <istreamwrapper.h>
#include <document.h>

#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"

#include <imgui.h>
#include "Component.h"
#include "MaterialManager.h"
#include "MyApplication.h"

Scene::Scene()
{
}

void Scene::AddGameObject(GameObject* pGameObject)
{
	m_pGameObjects.push_back(pGameObject);
	pGameObject->SetScene(this);
}

void Scene::RemoveGameobject(GameObject* pGameobject)
{
	m_pGameObjects.erase(std::remove(m_pGameObjects.begin(), m_pGameObjects.end(), pGameobject));
}

Scene::~Scene()
{
	for (auto iter = m_pGameObjects.begin(); iter < m_pGameObjects.end(); ++iter)
		delete* iter;

	for (auto iter = m_pMaterials.begin(); iter != m_pMaterials.end(); ++iter)
		delete iter->second;

	m_pMaterials.clear();
	m_pGameObjects.clear();
}


void Scene::AddMaterial(const std::string& name, Material* pMaterial)
{
	if (m_pMaterials[name] != nullptr)
	{
		delete pMaterial;
		return;
	}

	m_pMaterials[name] = pMaterial;
}

Material* Scene::GetMaterial(const std::string& name)
{
	return m_pMaterials[name];
}

Material* Scene::GetLatestMaterial()
{
	return m_pMaterials.rbegin()->second;
}

void Scene::Start()
{
	for (auto iter = m_pGameObjects.begin(); iter != m_pGameObjects.end(); iter++)
	{
		(*iter)->Start();
	}
}

void Scene::Render(Camera* pCamera)
{
	for (auto iter = m_pGameObjects.begin(); iter != m_pGameObjects.end(); iter++)
	{
		(*iter)->Render(pCamera);
	}
}

void Scene::RenderGUI()
{
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	static bool test_drag_and_drop = false;
	static int selection_mask = (1 << 2);
	int node_clicked = -1;


	for (int i = 0; i < m_pGameObjects.size(); ++i)
	{
		ImGuiTreeNodeFlags node_flags = base_flags;
		const bool is_selected = (selection_mask & (1 << i)) != 0;
		if (is_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		RenderGameobjectSceneGraph(m_pGameObjects[i], i, base_flags, node_clicked, test_drag_and_drop);

		//if (ImGui::Button(m_pGameObjects[i]->GetName().c_str()))
		//{
		//	m_pSelectedGameobject = m_pGameObjects[i];
		//}
	}

	if (node_clicked != -1)
	{
		if (ImGui::GetIO().KeyCtrl)
			selection_mask ^= (1 << node_clicked);
		else
			selection_mask = (1 << node_clicked);
	}

	if (m_pSelectedGameobject == nullptr) return;
	ImGui::BeginChild(m_pSelectedGameobject->GetName().c_str());
	m_pSelectedGameobject->RenderGUI();
	ImGui::EndChild();
}

void Scene::Update(float dt)
{
	for (auto iter = m_pGameObjects.begin(); iter != m_pGameObjects.end(); iter++)
	{
		(*iter)->Update(dt);
	}
}

void Scene::Serialize(const std::string& filename)
{
	std::ofstream levelFile{ filename + ".json" };
	if (!levelFile.is_open())
	{
		return;
	}
	rapidjson::StringBuffer outputFile{};
	rapidjson::PrettyWriter< rapidjson::StringBuffer> writer(outputFile);


	writer.StartObject();
	MaterialManager::GetInstance()->Serialize(writer);

	writer.Key("SceneName");
	writer.String(filename.c_str());

	writer.Key("Gameobjects");
	writer.StartArray();

	for (auto& pGameobject : m_pGameObjects)
	{
		writer.StartObject();
		pGameobject->Serialize(writer);
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();

	levelFile << outputFile.GetString();

	levelFile.close();
}

void Scene::Deserialize(const std::string& filename)
{
	m_pGameObjects.clear();

	std::ifstream levelFile{ filename + ".json" };
	if (!levelFile.is_open())
	{
		return;
	}
	rapidjson::IStreamWrapper isw{ levelFile };
	rapidjson::Document levelDocument{};
	levelDocument.ParseStream(isw);

	MaterialManager::GetInstance()->Deserialize(this, levelDocument);

	for (auto& gameobject : levelDocument["Gameobjects"].GetArray())
	{
		AddGameObject(GameObject::Deserialize(this, gameobject));
	}
}

void Scene::SetCamera(CameraComponent* pCameraComponent)
{
	m_pCameraComponent = pCameraComponent;
}

Camera* Scene::GetCamera() const
{
#ifdef _DEBUG
	if (MyEngine::GetSingleton()->GetPlaying())
		return m_pCameraComponent;
	else
		return MyEngine::GetSingleton()->GetApplication()->GetCamera();
#else
	return m_pCameraComponent;
#endif // _DEBUG


}

GameObject* Scene::GetSelectedObject() const
{
	return m_pSelectedGameobject;
}

void Scene::RenderGameobjectSceneGraph(GameObject* pGameobject, int i, ImGuiTreeNodeFlags node_flags, int& node_clicked, bool test_drag_and_drop)
{
	if (pGameobject->GetChildCount() > 0)
	{
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, pGameobject->GetName().c_str(), i);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			node_clicked = i;
			m_pSelectedGameobject = pGameobject;
		}
		if (test_drag_and_drop && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
			ImGui::Text("This is a drag and drop source");
			ImGui::EndDragDropSource();
		}
		if (node_open)
		{
			for (int childIndex = 0; childIndex < pGameobject->GetChildCount(); ++childIndex)
			{
				RenderGameobjectSceneGraph(pGameobject->GetChild(childIndex),childIndex, node_flags, node_clicked, test_drag_and_drop);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, pGameobject->GetName().c_str(), i);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			node_clicked = i;
			m_pSelectedGameobject = pGameobject;
		}
		if (test_drag_and_drop && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
			ImGui::Text("This is a drag and drop source");
			ImGui::EndDragDropSource();
		}
	}
}
