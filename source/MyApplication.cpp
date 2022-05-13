// MyApplication.cpp : Defines the entry point for the application.
//

#include "MyApplication.h"

#include "MyEngine.h"
#include "DataTypes.h"
#include "Mesh.h"
#include "Camera.h"
#include "OBJParser.h"
#include "Material.h"
#include "LitMaterial.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component.h"

#include "ServiceLocator.h"

#define MY_ENGINE MyEngine::GetSingleton()

MyApplication::MyApplication()
{
    MY_ENGINE->SetTitle(L"Engine");
}

MyApplication::~MyApplication()
{
	// nothing to destroy
	//delete m_pMesh;
	//for (auto& mesh : m_pMeshes)
	//{
	//	delete mesh;
	//	mesh = nullptr;
	//}

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

void MyApplication::RightMouseButtonAction(int x, int y, bool isUp)
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

void MyApplication::Paint()
{
	//if (!m_initialized)
	//	return;
	
	if (!MY_ENGINE->IsDirectXInitialized())
		return;
	
	// Clear Buffers
	MY_ENGINE->SetBackground(RGBColor{ 0.f, 0.f ,0.3f });

	// Matrices
	//MY_ENGINE->GetDeviceContext()->PSSetShaderResources()
	//MY_ENGINE->GetDevice().(D3DTRANSFORMSTATE_VIEW, &m_pCamera->GetViewMatrix())
	//m_pLitMaterial->GetWorldMatrix()->SetMatrix(&m_pMesh->GetWorldMatrix().data[0][0]);
	//m_pLitMaterial->GetViewInverseMatrix()->SetMatrix(&m_pCamera->GetViewMatrix().data[0][0]);

	// Render
	//m_pMesh->Render(MY_ENGINE->GetDeviceContext(), m_pCamera);
	//for (auto& mesh : m_pMeshes)
	//	mesh->Render(MY_ENGINE->GetDeviceContext(), m_pCamera);

	m_pScene->Render(m_pCamera);

	// Present
	MY_ENGINE->Present();
	
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

	for (auto& mesh : m_pMeshes)
	{
		GameObject* gameobject = new GameObject("Cube");
		
		m_pScene->AddGameObject(gameobject);

		gameobject->AddComponent(new MeshComponent(mesh));
		gameobject->AddComponent(new Rotator(45.f, FVector3{ 0,1,0 }));
	}

	m_pScene->GetMaterial("lambert8SG")->SetDiffuseMap(
		new Texture(ServiceLocator::GetDX11Renderer()->GetDevice(), L"Resources/T_BarrelAndBanjo_BC_01.jpg", ServiceLocator::GetDX11Renderer()->GetDeviceContext()));
	m_pScene->GetMaterial("lambert5SG")->SetDiffuseMap(
		new Texture(ServiceLocator::GetDX11Renderer()->GetDevice(), L"Resources/T_Distillery_BC_01.jpg", ServiceLocator::GetDX11Renderer()->GetDeviceContext()));
	m_pScene->GetMaterial("lambert9SG")->SetDiffuseMap(
		new Texture(ServiceLocator::GetDX11Renderer()->GetDevice(), L"Resources/T_Shotgun_BC_01.jpg", ServiceLocator::GetDX11Renderer()->GetDeviceContext()));
	m_pScene->GetMaterial("lambert10SG")->SetDiffuseMap(
		new Texture(ServiceLocator::GetDX11Renderer()->GetDevice(), L"Resources/T_ChairAndFirepit_BC_01.jpg", ServiceLocator::GetDX11Renderer()->GetDeviceContext()));
		m_pScene->GetMaterial("DAE2_RickAstley_Assignment1_000_aiStandardSurface1SG1")->SetDiffuseMap(
			new Texture(ServiceLocator::GetDX11Renderer()->GetDevice(), L"Resources/uv_grid_2.png", MY_ENGINE->GetDeviceContext()));

	//m_pScene->RemoveEntity(m_pTestEntity2);

	//m_pLitMaterial = new LitMaterial(MY_ENGINE->GetDevice(), L"Resources/material_lit.fx");
	//m_pUnLitMaterial = new Material(MY_ENGINE->GetDevice(), L"Resources/material_unlit.fx");

	//Texture* pDiffuseTexture = new Texture(MY_ENGINE->GetDevice(), L"Resources/uv_grid_2.png");
	//Texture* pNormalTexture = new Texture(MY_ENGINE->GetDevice(), L"Resources/vehicle_normal.png");
	//Texture* pSpecularTexture = new Texture(MY_ENGINE->GetDevice(), L"Resources/vehicle_specular.png");
	//Texture* pGlossinessTexture = new Texture(MY_ENGINE->GetDevice(), L"Resources/vehicle_gloss.png");
	//m_pLitMaterial->SetDiffuseMap(pDiffuseTexture);
	//m_pLitMaterial->SetNormalMap(pNormalTexture);
	//m_pLitMaterial->SetSpecularMap(pSpecularTexture);
	//m_pLitMaterial->SetGlossinessMap(pGlossinessTexture);
	//m_pUnLitMaterial->SetDiffuseMap(pDiffuseTexture);
	

	//m_pMesh = new Mesh(MY_ENGINE->GetDevice(), MY_ENGINE->GetWindowHandle(), "Resources/5Props.obj", m_pUnLitMaterial);
	//m_pMesh = new Mesh(MY_ENGINE->GetDevice(), MY_ENGINE->GetWindowHandle(), "Resources/vehicle.obj", m_pLitMaterial);

	//m_pLights.push_back(PointLight{ FVector3{-10.f, 0, 0}, FVector3{1,0,0}, 1.f });
	//m_pLights.push_back(PointLight{ FVector3{10.f, 0, 0}, FVector3{0,0,1}, 1.f });
	//
	//for (int i = 0; i < m_pLights.size(); ++i)
	//	m_pLitMaterial->SetLight(i, m_pLights[i]);
	//
	//m_pLitMaterial->GetNrOfLightsVariable()->SetInt(m_pLights.size());

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
