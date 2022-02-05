// MyApplication.cpp : Defines the entry point for the application.
//

#include "MyApplication.h"

#include "MyEngine.h"
#include "DataTypes.h"
#include "Mesh.h"
#include "Camera.h"
#include "OBJParser.h"
#include "Material.h"

#define MY_ENGINE MyEngine::GetSingleton()

MyApplication::MyApplication()
{
    MY_ENGINE->SetTitle(L"Triangle");
}

MyApplication::~MyApplication()
{
	// nothing to destroy
	delete m_pMesh;
	delete m_pCamera;
}

//-------------------------------------------------
// Own methods								
//-------------------------------------------------
void MyApplication::LeftMouseButtonAction(int x, int y, bool isUp)
{
	if (isUp) m_PointsVec.push_back({ x, y });
	m_pCamera->SetLeftMouseButtonPressed(isUp);
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

	// Render
	m_pMesh->Render(MY_ENGINE->GetDeviceContext(), m_pCamera);

	// Present
	MY_ENGINE->Present();
	
}

void MyApplication::Update(float dt)
{
	m_pCamera->Update(dt);
}

void MyApplication::Initialize()
{
	//ParseOBJ("Resources/vehicle.obj", vertices, indices);

	RECT rect;
	GetWindowRect(MY_ENGINE->GetWindowHandle(), &rect);
	float width = rect.right - rect.left;
	float height = rect.bottom - rect.top;
	m_pCamera = new Camera(FVector3{ 0,0,0 }, FVector3{ 0,0,1 }, 60.f, static_cast<float>(width) / static_cast<float>(height));

	Material* pMaterial = new Material(MY_ENGINE->GetDevice(), L"Resources/material_unlit.fx");
	Texture* pTexture = new Texture(MY_ENGINE->GetDevice(), L"Resources/uv_grid_2.png");
	pMaterial->SetDiffuseMap(pTexture);
	
	m_pMesh = new Mesh(MY_ENGINE->GetDevice(), MY_ENGINE->GetWindowHandle(), "Resources/vehicle.obj", pMaterial);
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
