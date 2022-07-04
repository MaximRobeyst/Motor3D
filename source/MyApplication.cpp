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

	delete m_pCamera;
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

void MyApplication::KeyUp(WPARAM )
{
	//m_pCamera->KeyUp(wparam);
}

void MyApplication::KeyDown(WPARAM )
{
	//m_pCamera->KeyDown(wparam);
}

void MyApplication::SaveFile()
{
}

void MyApplication::LoadFile()
{
	
}

void MyApplication::Start()
{
	m_pScene->Start();
}

void MyApplication::Render()
{
	m_pScene->Render(m_pCamera);	
}

void MyApplication::RenderGUI()
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_MenuBar;

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
		ImGui::EndMenuBar();
	}


	if (ImGui::CollapsingHeader("Scene"))
	{
		m_pScene->RenderGUI();
	}


	ImGui::End();
}

void MyApplication::Update(float dt)
{

	//m_pCamera->Update(dt);

	//m_pMesh->SetWorldMatrix(rotationMatrix);

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
	pCamera->AddComponent(new CameraComponent(60, static_cast<float>(width) / static_cast<float>(height), 2500.f, 0.1f));
	m_pScene->AddGameObject(pCamera);


	ParseOBJ("Resources/5Props.obj", m_pMeshes);

	//for (int i = 0; i < 10; i++)
	//{
	//	for (int j = 0; j < 10; j++)
	//	{
	//		GameObject* cube = new GameObject("Cube", FVector3{  RandomFloat(100.f, -100.f), RandomFloat(50.f, 40.f), RandomFloat(100.f, -100.f)});
	//
	//		m_pScene->AddGameObject(cube);
	//
	//		Material* mat = new Material(MY_ENGINE->GetDevice(), L"Resources/material_unlit.fx");
	//		Texture* pDiffuseTexture = new Texture(MY_ENGINE->GetDevice(), L"Resources/uv_grid_2.png");
	//		mat->SetDiffuseMap(pDiffuseTexture);
	//		Mesh* pMesh = new Mesh(MY_ENGINE->GetDevice(), MY_ENGINE->GetWindowHandle(), "Resources/cube.obj", mat);
	//
	//		cube->AddComponent(new MeshComponent(pMesh));
	//		cube->AddComponent(new RigidBodyComponent(FVector3{}, FVector3{}, FVector3{ 0.f,-9.81f, 0.f }));
	//	}
	//}

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

void MyApplication::Activate(bool isActive)
{
	if (isActive) 
        MY_ENGINE->SetTitle(L"Slayer");
	else 
        MY_ENGINE->SetTitle(L"This window is inactive; go away.");
}
