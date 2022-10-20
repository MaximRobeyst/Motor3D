#include "PhysxProxy.h"
#include "Logger.h"

#include <cassert>
#include <DirectXMath.h>
#include "PhysxHelper.h"
#include "Component.h"
#include "GameTime.h"
#include "MyEngine.h"
#include "Scene.h"
#include "DebugRenderer.h"

using namespace DirectX;

bool PhysxProxy::m_PhysXFrameStepping{ false };
float PhysxProxy::m_PhysXStepTime{ 0.f };

PhysxProxy::~PhysxProxy()
{
	if (m_pControllerManager != nullptr)
		m_pControllerManager->release();
	if (m_pPhysxScene != nullptr)
		m_pPhysxScene->release();
}

void PhysxProxy::Initialize(Scene* pParent)
{
	m_pScene = pParent;
	if (m_IsInitialized)
	{
		Logger::GetInstance()->LogWarning("Multiple Initialization attempts!");
		return;
	}

	m_pPhysxScene = PhysXManager::GetInstance()->CreateScene(pParent);
	if (!m_pPhysxScene)
		Logger::GetInstance()->LogErrorAndBreak("Failed to create physx scene!");

	m_pPhysxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	m_pPhysxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	m_pPhysxScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	m_pPhysxScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	m_pPhysxScene->setSimulationEventCallback(this);

	if (!m_pPhysxScene) return; //Prevent C6011
	m_pControllerManager = PxCreateControllerManager(*m_pPhysxScene);
	assert(m_pControllerManager != nullptr);

	m_IsInitialized = true;
}

void PhysxProxy::Update() const
{
	if (MyEngine::GetSingleton()->GetPlaying() && GameTime::GetInstance().GetElapsed() > 0)
	{
		if (m_PhysXFrameStepping)
		{
			if (m_PhysXStepTime > 0.f)
			{
				m_pPhysxScene->simulate(m_PhysXStepTime);
				m_pPhysxScene->fetchResults(true);
				m_PhysXStepTime = 0.f;
			}
			else if (m_PhysXStepTime < 0.f)
			{
				m_pPhysxScene->simulate(GameTime::GetInstance().GetElapsed());
				m_pPhysxScene->fetchResults(true);
			}
		}
		else
		{
			m_pPhysxScene->simulate(GameTime::GetInstance().GetElapsed());
			m_pPhysxScene->fetchResults(true);
		}
	}

#ifdef _DEBUG
	//Send Camera to PVD
	if (m_pPhysxScene->getScenePvdClient())
	{
		const Camera* pCamera = m_pScene->GetCamera();
		const auto position = pCamera->GetPosition();
		const auto forward = pCamera->GetForward();
		const auto up = pCamera->GetUp();

		DirectX::XMFLOAT3 cameraTarget{};
		DirectX::XMStoreFloat3(&cameraTarget, DirectX::XMLoadFloat3(&position) + (DirectX::XMLoadFloat3(&forward) * 10.f));
		m_pPhysxScene->getScenePvdClient()->updateCamera("SceneCam", PhysxHelper::ToPxVec3(position), PhysxHelper::ToPxVec3(position), PhysxHelper::ToPxVec3(cameraTarget));
	}
#endif
}

void PhysxProxy::Draw() const
{
	DebugRenderer::GetInstance()->DrawPhysX(m_pPhysxScene);
}

bool PhysxProxy::Raycast(const physx::PxVec3& origin, const physx::PxVec3& unitDir, physx::PxReal distance, physx::PxRaycastCallback& hitCall, physx::PxHitFlags hitFlags, const physx::PxQueryFilterData& filterData, physx::PxQueryFilterCallback* filterCall, const physx::PxQueryCache* cache) const
{
	if (m_pPhysxScene != nullptr)
	{
		return m_pPhysxScene->raycast(origin, unitDir, distance, hitCall, hitFlags, filterData, filterCall, cache);
	}

	Logger::GetInstance()->LogWarning("Raycast failed. Physics scene is a null pointer");
	return false;
}

void PhysxProxy::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::
			eREMOVED_SHAPE_OTHER))
			continue;

		const auto triggerComponent = reinterpret_cast<IComponent*>(((pairs[i].triggerShape)->getActor())->userData);
		const auto otherComponent = reinterpret_cast<IComponent*>(((pairs[i].otherShape)->getActor())->userData);


		if (triggerComponent != nullptr && otherComponent != nullptr)
		{
			GameObject* trigger = triggerComponent->GetGameObject();
			GameObject* other = otherComponent->GetGameObject();

			if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
				trigger->OnTriggerEnter(other);
			else if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
				trigger->OnTriggerExit( other);
		}
	}
}
