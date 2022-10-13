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

#include "EditorWindow.h"
#include "LogWindow.h"

#include "Scene.h"

void DemoApplication::Initialize()
{
	LoadBaseScene();
	//LoadPongScene();
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
	auto pRigidBody = new RigidBodyComponent(true);
	auto pPeddle = new GameObject("Peddle", DirectX::XMFLOAT3{ 5.f, 0.f, 0.f });
	pPeddle->AddComponent(new MeshComponent(CreateCube(.75f, 2.f, .75f)));
	pPeddle->AddComponent(pRigidBody);
	auto colliderId = pRigidBody->AddCollider(physx::PxBoxGeometry{ 0.75f / 2.f, 1.f, .75f / 2.f }, *pDefaultMaterial, true);

	m_pScene->AddGameObject(pPeddle);

	pRigidBody = new RigidBodyComponent(true);
	pPeddle = new GameObject("Peddle 2", DirectX::XMFLOAT3{ -5.f, 0.f, 0.f });
	pPeddle->AddComponent(new MeshComponent(CreateCube(.75f, 2.f, .75f)));
	pPeddle->AddComponent(pRigidBody);
	colliderId = pRigidBody->AddCollider(physx::PxBoxGeometry{ 0.75f / 2.f, 1.f, .75f / 2.f }, *pDefaultMaterial, true);

	m_pScene->AddGameObject(pPeddle);

	pRigidBody = new RigidBodyComponent(true);
	auto pBall = new GameObject("Ball");
	pBall->AddComponent(new MeshComponent(CreateSphere(0.25f, 10)));
	pBall->AddComponent(pRigidBody);

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
