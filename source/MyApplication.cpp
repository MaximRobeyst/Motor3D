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
#include "MeshComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "ParticleComponent.h"
#include "TerrainComponent.h"

#include "Logger.h"

#include "EditorWindow.h"
#include "LogWindow.h"

#include <iostream>
#include <filesystem>
#include "DebugRenderer.h"

#define MY_ENGINE MyEngine::GetSingleton()

MyApplication::MyApplication()
{
    MY_ENGINE->SetTitle(L"Engine");
}

MyApplication::~MyApplication()
{
	m_pMeshes.clear();


#ifdef _DEBUG
	delete m_pCamera;
	m_pCamera = nullptr;

	for (auto& window : m_pEditorWindows)
	{
		window->Cleanup();
		delete window;
	}
	m_pEditorWindows.clear();
#endif // _DEBUG

	delete m_pScene;

	delete MaterialManager::GetInstance();
	delete ResourceManager::GetInstance();
	delete PhysXManager::GetInstance();
	delete DebugRenderer::GetInstance();
}

//-------------------------------------------------
// Own methods								
//-------------------------------------------------
void MyApplication::LeftMouseButtonAction()
{
}

void MyApplication::RightMouseButtonAction()
{
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
EditorWindow* MyApplication::GetWindowByName(std::string name)
{
	auto iter = std::find_if(m_pEditorWindows.begin(), m_pEditorWindows.end(), [&](EditorWindow* editorWindow)
		{
			return name == editorWindow->GetTitle();
		});

	return *iter;
}

void MyApplication::AddWindowEditor(EditorWindow* pWindow)
{
	m_pEditorWindows.emplace_back(pWindow);
}

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


void MyApplication::BaseStart()
{
#ifdef _DEBUG
	if (m_pCamera == nullptr)
		m_pCamera = new Camera(DirectX::XMFLOAT3{ 0,1,-2.5f }, DirectX::XMFLOAT3{ 0,0,1 }, (F_PI / 4.f), MyEngine::GetSingleton()->GetWindowWidth() / MyEngine::GetSingleton()->GetWindowHeight());

	for (auto editorWindow : m_pEditorWindows)
	{
		editorWindow->Initialize();
	}

#endif
	m_pScene->Start();
	Start();
}

void MyApplication::BaseRender()
{
	m_pScene->Render(nullptr);	
	Render();
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
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("New Gameobject"))
				m_pScene->AddGameObject(new GameObject("New Gameobject"));
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

	for (auto editorWindow : m_pEditorWindows)
	{
		editorWindow->RootDraw();
	}
}
#endif

void MyApplication::BaseUpdate()
{
#ifdef _DEBUG
	m_pCamera->UpdateCamera();
#endif // _DEBUG

	if (!MyEngine::GetSingleton()->GetPlaying()) return;
	m_pScene->Update();

	// Center Cursor
	RECT windowRect;
	GetWindowRect(MY_ENGINE->GetWindowHandle(), &windowRect);

	DirectX::XMFLOAT2 windowCenter{ static_cast<float>( windowRect.left + ((windowRect.right - windowRect.left) / 2)), static_cast<float>(windowRect.bottom + ((windowRect.top - windowRect.bottom) / 2)) };
	Update();
}

void MyApplication::BaseInitialize()
{
	Initialize();
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
