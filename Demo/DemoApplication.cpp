#include "DemoApplication.h"

#include "MyEngine.h"
#pragma warning (push, 0)
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
#include <RigidbodyComponent.h>

#include "Logger.h"
#include "BallComponent.h"

#include "EditorWindow.h"
#include "LogWindow.h"

#include "Scene.h"
#include "DebugRenderer.h"
#include "BrickComponent.h"

void DemoApplication::Initialize()
{
	//LoadBaseScene();
	LoadPongScene();
}

void DemoApplication::Update()
{
	DebugRenderer::GetInstance()->DrawLine(DirectX::XMFLOAT3{ 0,0,0 }, DirectX::XMFLOAT3{ 0,1,0 }, DirectX::XMFLOAT4{ 1,0,0,0 });
}

void DemoApplication::LoadPongScene()
{
	m_pScene = new Scene();
	auto pCamera = new GameObject("Camera", DirectX::XMFLOAT3{ 0, 0, -9.0f });
	pCamera->AddComponent(new CameraComponent(F_PI / 4.f, static_cast<float>(MyEngine::GetSingleton()->GetWindowWidth()) / static_cast<float>(MyEngine::GetSingleton()->GetWindowHeight()), 100.f, 0.1f));
	m_pScene->AddGameObject(pCamera);

	auto pMaterialManager = MaterialManager::GetInstance();
	pMaterialManager->AddMaterial("default", new Material(MyEngine::GetSingleton()->GetDevice(), "Resources/material_unlit.fx", "default"));
	pMaterialManager->GetMaterial("default")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/uv_grid_2.png", MyEngine::GetSingleton()->GetDeviceContext()));

	auto pDefaultMaterial = PxGetPhysics().createMaterial(.5f, .5f, 1.f);
	auto pRigidBody = new RigidBodyComponent(false);
	pRigidBody->SetKinematic(true);
	auto pPlayer = new GameObject("Player", DirectX::XMFLOAT3{ 0.f, -2.f, 0.f });
	pPlayer->AddComponent(new MeshComponent(CreateCube(0.5, 0.1f, 0.5f)));
	pPlayer->AddComponent(pRigidBody);
	pRigidBody->AddCollider(physx::PxBoxGeometry{ 0.25f, 0.05f, 0.25f }, *pDefaultMaterial, true);

	constexpr int gridHeight = 10;
	constexpr int gridWidth = 5;

	//float startPos = -gridHeight / 2.f;
	//float startPosY = -gridHeight / 2.f + 3.0f;


	GameObject* pGrid = new GameObject("Grid", DirectX::XMFLOAT3{-4.0f, 0.0f, 0.0f});
	m_pScene->AddGameObject(pGrid);

	for (int i = 0; i < gridHeight; ++i)
	{
		for (int j = 0; j < gridWidth; ++j)
		{
			auto pBrick = new GameObject("Grid", DirectX::XMFLOAT3{static_cast<float>(i), static_cast<float>(j) * 0.5f, 0.f});
			pRigidBody = new RigidBodyComponent(true);

			pBrick->AddComponent(new MeshComponent(CreateCube(0.5, 0.1f, 0.5f)));
			pBrick->AddComponent(pRigidBody);
			pRigidBody->AddCollider(physx::PxBoxGeometry{ 0.25f, 0.05f, 0.25f }, *pDefaultMaterial, true);

			pBrick->AddComponent(new BrickComponent());

			pBrick->SetParent(pGrid);

		}
	}

	m_pScene->AddGameObject(pPlayer);

	pRigidBody = new RigidBodyComponent(false);
	pRigidBody->SetKinematic(true);
	auto pBall = new GameObject("Ball", DirectX::XMFLOAT3{0,-1,0});
	pBall->AddComponent(new MeshComponent(CreateSphere(0.1f, 10)));
	pBall->AddComponent(pRigidBody);
	pRigidBody->AddCollider(physx::PxSphereGeometry(0.1f), *pDefaultMaterial, false);

	pBall->AddComponent(new BallComponent());

	m_pScene->AddGameObject(pBall);
}

void DemoApplication::LoadBaseScene()
{
	//ParseOBJ("Resources/vehicle.obj", vertices, indices);
	Logger::GetInstance()->Log(LogLevel::Debug, "Log test");

	auto pMaterialManager = MaterialManager::GetInstance();

	pMaterialManager->AddMaterial("default", new Material(MyEngine::GetSingleton()->GetDevice(), "Resources/material_unlit.fx", "default"));
	pMaterialManager->GetMaterial("default")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/uv_grid_2.png", MyEngine::GetSingleton()->GetDeviceContext()));

	RECT rect;
	GetWindowRect(MyEngine::GetSingleton()->GetWindowHandle(), &rect);
	float width = static_cast<float>(rect.right - rect.left);
	float height = static_cast<float>(rect.bottom - rect.top);
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

		if (i == 0)
			m_pScene->AddGameObject(gameobjects[i]);
		else
			gameobjects[i]->SetParent(gameobjects[i - 1]);

		gameobjects[i]->AddComponent(new MeshComponent(mesh));
		gameobjects[i]->AddComponent(new Rotator(45.f, DirectX::XMFLOAT3{ 0,1,0 }));

		++i;
	}

	m_pMeshes.clear();
	ParseOBJ("Resources/Models/biplane.obj", m_pMeshes);

	for (auto& mesh : m_pMeshes)
	{
		gameobjects.emplace_back(new GameObject("Plane" + std::to_string(i)));

		m_pScene->AddGameObject(gameobjects[i]);

		gameobjects[i]->AddComponent(new MeshComponent(mesh));
		gameobjects[i]->AddComponent(new Rotator(45.f, DirectX::XMFLOAT3{ 0,1,0 }));

		++i;
	}

	auto spriteObject = new GameObject("Sprite");
	m_pScene->AddGameObject(spriteObject);
	auto spriteComponent = new SpriteComponent();
	spriteComponent->SetTexture("Resources/uv_grid_2.png");
	spriteObject->AddComponent(spriteComponent);
	spriteObject->GetTransform()->SetScale(DirectX::XMFLOAT3{ 0.25f, 0.25f, 0.25f });

	auto particleObject = new GameObject("Particle Object");
	m_pScene->AddGameObject(particleObject);
	ParticleEmmiterSettings particleSettings{};
	particleObject->AddComponent(new ParticleComponent("Resources/smoke.png", particleSettings, 60));

	auto terrainObject = new GameObject("Terrain Object");
	m_pScene->AddGameObject(terrainObject);
	terrainObject->AddComponent(new MeshComponent());
	terrainObject->AddComponent(new TerrainComponent(64, 64));

	pMaterialManager->GetMaterial("lambert8SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/T_BarrelAndBanjo_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	pMaterialManager->GetMaterial("lambert5SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/T_Distillery_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	pMaterialManager->GetMaterial("lambert9SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/T_Shotgun_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	pMaterialManager->GetMaterial("lambert10SG")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/T_ChairAndFirepit_BC_01.jpg", MyEngine::GetSingleton()->GetDeviceContext()));
	pMaterialManager->GetMaterial("DAE2_RickAstley_Assignment1_000_aiStandardSurface1SG1")->SetDiffuseMap(
		new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/uv_grid_2.png", MyEngine::GetSingleton()->GetDeviceContext()));
}
