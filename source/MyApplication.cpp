// MyApplication.cpp : Defines the entry point for the application.
//

#include "MyApplication.h"

#include "MyEngine.h"

#pragma warning (push, 0)
#include "DataTypes.h"
#include "OBJParser.h"
#pragma warning(pop)

#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "LitMaterial.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include <imgui.h>
#include "ResourceManager.h"
#include "Utils.h"

#include <iostream>
#include <filesystem>

#define MY_ENGINE MyEngine::GetSingleton()

MyApplication::MyApplication()
{
    MY_ENGINE->SetTitle(L"Engine");
}

MyApplication::~MyApplication()
{
	m_pMeshes.clear();

	delete MaterialManager::GetInstance();
	delete ResourceManager::GetInstance();

#ifdef _DEBUG
	delete m_pCamera;
#endif // _DEBUG

	delete m_pScene;
}

//-------------------------------------------------
// Own methods								
//-------------------------------------------------
void MyApplication::LeftMouseButtonAction(int x, int y, bool isUp)
{
	if (isUp) m_PointsVec.push_back({ x, y });
}

void MyApplication::RightMouseButtonAction(int , int , bool isUp)
{
	if (isUp) m_PointsVec.clear();
}

void MyApplication::KeyUp(WPARAM wparam)
{
#ifdef _DEBUG
	if(!MyEngine::GetSingleton()->GetPlaying())
		m_pCamera->KeyUp(wparam);
#endif
}

void MyApplication::KeyDown(WPARAM wparam)
{

#ifdef _DEBUG
	if (!MyEngine::GetSingleton()->GetPlaying())
		m_pCamera->KeyDown(wparam);
#endif
}

#ifdef _DEBUG
Camera* MyApplication::GetCamera() const
{
	return m_pCamera;
}

void MyApplication::ApplicationFiles(const std::string& path)
{
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	static bool test_drag_and_drop = false;
	static int selection_mask = (1 << 2);
	int node_clicked = -1;

	int i{};
	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		std::string filepath{ file.path().string() };
		if (filepath.substr(filepath.find_last_of(".") + 1) == "obj")
		{
			ResourceManager::GetInstance()->AddMeshFile(filepath);
		}

		DialogueFolder(i, filepath, base_flags, node_clicked, test_drag_and_drop);
		++i;
	}

	if (node_clicked != -1)
	{
		if (ImGui::GetIO().KeyCtrl)
			selection_mask ^= (1 << node_clicked);
		else
			selection_mask = (1 << node_clicked);
	}
}
void MyApplication::DialogueFolder(int i, const std::string& path, ImGuiTreeNodeFlags node_flags, int& node_clicked, bool test_drag_and_drop)
{
	std::wstring wpath{ path.begin(), path.end() };
	if (GetFileAttributes(wpath.c_str()) == FILE_ATTRIBUTE_DIRECTORY)
	{
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, path.c_str(), i);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			node_clicked = i;
		}
		if (test_drag_and_drop && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
			ImGui::Text("This is a drag and drop source");
			ImGui::EndDragDropSource();
		}
		if (node_open)
		{
			int j{};
			for (const auto& file : std::filesystem::directory_iterator(path))
			{
				DialogueFolder(j, file.path().string(), node_flags, node_clicked, test_drag_and_drop);
				++j;
			}
			ImGui::TreePop();
		}
	}
	else
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, path.c_str(), i);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			node_clicked = i;
		}
		if (test_drag_and_drop && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
			ImGui::Text("This is a drag and drop source");
			ImGui::EndDragDropSource();
		}
	}
}
#endif // _DEBUG


void MyApplication::Start()
{
#ifdef _DEBUG
	m_pCamera = new Camera(DirectX::XMFLOAT3{ 0,1,-2.5f }, DirectX::XMFLOAT3{ 0,0,1 }, (F_PI / 4.f), MyEngine::GetSingleton()->GetWindowWidth() / MyEngine::GetSingleton()->GetWindowHeight());
#endif
	m_pScene->Start();
}

void MyApplication::Render()
{
	m_pScene->Render(nullptr);	
}

#ifdef _DEBUG
void MyApplication::RenderGUI()
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_MenuBar;

	ImGui::Begin("Files");
	ApplicationFiles("Resources/");
	ImGui::End();

	ImGui::Begin("Application", 0, windowFlags);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Save as..."))
			{
				static char filename[128] = "New_File";
				ImGui::InputText("Filename:", filename, 128);
				if (ImGui::Button("Save"))
				{
					m_pScene->Serialize(filename);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Load"))
			{
				static char filename[128] = "New_File";
				ImGui::InputText("Filename:", filename, 128);
				if (ImGui::Button("Load"))
				{
					delete m_pScene;

					m_pScene = new Scene();
					m_pScene->Deserialize(filename);
					m_pScene->Start();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::BeginMenu("Debug Camera"))
			{
				m_pCamera->CameraGui();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("In Window"))
			{
				bool inWindow = MyEngine::GetSingleton()->GetInWindow();
				if(ImGui::Checkbox("In Window", &inWindow))
				{
					MyEngine::GetSingleton()->SetInWindow(inWindow);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	if (ImGui::Button("Play"))
		MyEngine::GetSingleton()->SetPlaying(true);
	ImGui::SameLine();
	if(ImGui::Button("Pause"))
		MyEngine::GetSingleton()->SetPlaying(false);

	if (ImGui::CollapsingHeader("Scene"))
	{
		m_pScene->RenderGUI();
	}


	ImGui::End();
}
#endif

void MyApplication::Update(float dt)
{
#ifdef _DEBUG
	m_pCamera->UpdateCamera(dt);
#endif // _DEBUG


	//m_pCamera->Update(dt);

	//m_pMesh->SetWorldMatrix(rotationMatrix);

	if (!MyEngine::GetSingleton()->GetPlaying()) return;
	m_pScene->Update(dt);

	// Center Cursor
	RECT windowRect;
	GetWindowRect(MY_ENGINE->GetWindowHandle(), &windowRect);

	DirectX::XMFLOAT2 windowCenter{ static_cast<float>( windowRect.left + ((windowRect.right - windowRect.left) / 2)), static_cast<float>(windowRect.bottom + ((windowRect.top - windowRect.bottom) / 2)) };

	//SetCursorPos(windowCenter.x, windowCenter.y);

}

void MyApplication::Initialize()
{
	//ParseOBJ("Resources/vehicle.obj", vertices, indices);

	RECT rect;
	GetWindowRect(MY_ENGINE->GetWindowHandle(), &rect);
	float width = static_cast<float>(rect.right - rect.left);
	float height =static_cast<float>( rect.bottom - rect.top);
	//m_pCamera = new Camera(DirectX::XMFLOAT3{ 0,1.f,-2.5 }, DirectX::XMFLOAT3{ 0,0,1 }, 60.f, static_cast<float>(width) / static_cast<float>(height));

	m_pScene = new Scene();
	auto pCamera = new GameObject("Camera", DirectX::XMFLOAT3{ 0, 1.f, -2.5f });
	pCamera->AddComponent(new CameraComponent(F_PI / 4.f, static_cast<float>(width) / static_cast<float>(height), 100.f, 0.1f));
	m_pScene->AddGameObject(pCamera);


	ParseOBJ("Resources/5Props.obj", m_pMeshes);

	int i{};
	std::vector<GameObject*> gameobjects{};
	for (auto& mesh : m_pMeshes)
	{
		gameobjects.emplace_back(new GameObject("Cube" + std::to_string(i)));
		
		if(i == 0)
			m_pScene->AddGameObject(gameobjects[i]);
		else
			gameobjects[i]->SetParent(gameobjects[i - 1]);

		gameobjects[i]->AddComponent(new MeshComponent(mesh));
		gameobjects[i]->AddComponent(new Rotator(45.f, DirectX::XMFLOAT3{0,1,0}));

		++i;
	}
	
	auto pMaterialManager = MaterialManager::GetInstance();

	pMaterialManager->AddMaterial("default", new Material(MyEngine::GetSingleton()->GetDevice(), "Resources/material_unlit.fx", "default"));
	pMaterialManager->GetMaterial("default")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/uv_grid_2.png", MY_ENGINE->GetDeviceContext()));


	pMaterialManager->GetMaterial("lambert8SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/T_BarrelAndBanjo_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	pMaterialManager->GetMaterial("lambert5SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/T_Distillery_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	pMaterialManager->GetMaterial("lambert9SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/T_Shotgun_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	pMaterialManager->GetMaterial("lambert10SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/T_ChairAndFirepit_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	pMaterialManager->GetMaterial("DAE2_RickAstley_Assignment1_000_aiStandardSurface1SG1")->SetDiffuseMap(
			new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/uv_grid_2.png", MY_ENGINE->GetDeviceContext()));

	m_pUnLitMaterial = nullptr;
}

void MyApplication::Unitialize()
{
}

Scene* MyApplication::GetScene()
{
	return m_pScene;
}

void MyApplication::Activate(bool isActive)
{
	if (isActive) 
        MY_ENGINE->SetTitle(L"Slayer");
	else 
        MY_ENGINE->SetTitle(L"This window is inactive; go away.");
}
