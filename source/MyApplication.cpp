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
	m_pLitMaterial->GetWorldMatrix()->SetMatrix(&m_pMesh->GetWorldMatrix().data[0][0]);
	m_pLitMaterial->GetViewInverseMatrix()->SetMatrix(&m_pCamera->GetViewMatrix().data[0][0]);

	// Render
	m_pMesh->Render(MY_ENGINE->GetDeviceContext(), m_pCamera);

	// Present
	MY_ENGINE->Present();
	
}

void MyApplication::Update(float dt)
{

	m_pCamera->Update(dt);

	m_Rotation += (ToRadians(45.f) * dt);
	FMatrix4 rotationMatrix = MakeRotation(m_Rotation, FVector3{ 0,1,0 });

	m_pMesh->SetWorldMatrix(rotationMatrix);

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
	float width = rect.right - rect.left;
	float height = rect.bottom - rect.top;
	m_pCamera = new Camera(FVector3{ 0,0,-50 }, FVector3{ 0,0,1 }, 60.f, static_cast<float>(width) / static_cast<float>(height));

	m_pLitMaterial = new LitMaterial(MY_ENGINE->GetDevice(), L"Resources/material_lit.fx");
	Texture* pDiffuseTexture = new Texture(MY_ENGINE->GetDevice(), L"Resources/vehicle_diffuse.png");
	Texture* pNormalTexture = new Texture(MY_ENGINE->GetDevice(), L"Resources/vehicle_normal.png");
	Texture* pSpecularTexture = new Texture(MY_ENGINE->GetDevice(), L"Resources/vehicle_specular.png");
	Texture* pGlossinessTexture = new Texture(MY_ENGINE->GetDevice(), L"Resources/vehicle_gloss.png");
	m_pLitMaterial->SetDiffuseMap(pDiffuseTexture);
	m_pLitMaterial->SetNormalMap(pNormalTexture);
	m_pLitMaterial->SetSpecularMap(pSpecularTexture);
	m_pLitMaterial->SetGlossinessMap(pGlossinessTexture);
	
	m_pMesh = new Mesh(MY_ENGINE->GetDevice(), MY_ENGINE->GetWindowHandle(), "Resources/vehicle.obj", m_pLitMaterial);

	m_pLights.push_back(PointLight{ FVector3{-10.f, 0, 0}, FVector3{1,0,0}, 1.f });
	m_pLights.push_back(PointLight{ FVector3{10.f, 0, 0}, FVector3{0,0,1}, 1.f });

	for (int i = 0; i < m_pLights.size(); ++i)
		m_pLitMaterial->SetLight(i, m_pLights[i]);

	m_pLitMaterial->GetNrOfLightsVariable()->SetInt(m_pLights.size());
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
