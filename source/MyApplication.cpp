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

#define MY_ENGINE MyEngine::GetSingleton()

MyApplication::MyApplication()
{
    MY_ENGINE->SetTitle(L"Engine");
}

MyApplication::~MyApplication()
{
	m_pMeshes.clear();

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

void MyApplication::KeyUp(WPARAM wparam)
{
	m_pCamera->KeyUp(wparam);
}

void MyApplication::KeyDown(WPARAM wparam)
{
	m_pCamera->KeyDown(wparam);
}

void MyApplication::Render()
{
	m_pScene->Render(m_pCamera);	
}

void MyApplication::RenderGUI()
{
	ImGui::Begin("Application");

	if (ImGui::CollapsingHeader("Scene"))
	{
		m_pScene->RenderGUI();
	}


	ImGui::End();
}

void MyApplication::Update(float dt)
{

	m_pCamera->Update(dt);

	m_Rotation += (ToRadians(45.f) * dt);
	FMatrix4 rotationMatrix = MakeRotation(m_Rotation, FVector3{ 0,1,0 });

	//m_pMesh->SetWorldMatrix(rotationMatrix);

	m_pScene->Update(dt);

	// Center Cursor
	RECT windowRect;
	GetWindowRect(MY_ENGINE->GetWindowHandle(), &windowRect);

	IVector2 windowCenter{ windowRect.left + ((windowRect.right - windowRect.left) / 2), windowRect.bottom + ((windowRect.top - windowRect.bottom) / 2) };

	//SetCursorPos(windowCenter.x, windowCenter.y);

}

void MyApplication::Initialize()
{
	//ParseOBJ("Resources/vehicle.obj", vertices, indices);

	RECT rect;
	GetWindowRect(MY_ENGINE->GetWindowHandle(), &rect);
	float width = static_cast<float>(rect.right - rect.left);
	float height =static_cast<float>( rect.bottom - rect.top);
	m_pCamera = new Camera(FVector3{ 0,1.f,-2.5 }, FVector3{ 0,0,1 }, 60.f, static_cast<float>(width) / static_cast<float>(height));

	m_pScene = new Scene();
	ParseOBJ("Resources/5Props.obj", m_pMeshes, m_pScene);

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
		
		m_pScene->AddGameObject(gameobjects[i]);
		gameobjects[i]->SetParent(gameobjects[0]);

		gameobjects[i]->AddComponent(new MeshComponent(mesh));
		gameobjects[i]->AddComponent(new Rotator(45.f, FVector3{0,1,0}));

		++i;
	}

	m_pScene->GetMaterial("lambert8SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), L"Resources/T_BarrelAndBanjo_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	m_pScene->GetMaterial("lambert5SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), L"Resources/T_Distillery_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	m_pScene->GetMaterial("lambert9SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), L"Resources/T_Shotgun_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	m_pScene->GetMaterial("lambert10SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), L"Resources/T_ChairAndFirepit_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
		m_pScene->GetMaterial("DAE2_RickAstley_Assignment1_000_aiStandardSurface1SG1")->SetDiffuseMap(
			new Texture(MyEngine::GetSingleton()->GetDevice(), L"Resources/uv_grid_2.png", MY_ENGINE->GetDeviceContext()));

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
