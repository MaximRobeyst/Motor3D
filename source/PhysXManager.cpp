#include "PhysXManager.h"
#include "PhysxAllocator.h"
#include "PhysxErrorCallback.h"
#include "OverlordSimulationFilterShader.h"

#include <cassert>
#include "Logger.h"

#define PVD_HOST "127.0.0.1"
#define PVD_PORT 5425

PhysXManager* PhysXManager::m_pPhysxManager;

physx::PxScene* PhysXManager::CreateScene(Scene* pScene) const
{
	auto sceneDesc = physx::PxSceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.setToDefault(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = m_pDefaultCpuDispatcher;
	sceneDesc.cudaContextManager = m_pCudaContextManager;
	sceneDesc.filterShader = OverlordSimulationFilterShader;
	sceneDesc.userData = pScene;

	const auto physxScene = m_pPhysics->createScene(sceneDesc);
	assert(physxScene != nullptr);

	return physxScene;
}

PhysXManager* PhysXManager::GetInstance()
{
	if (m_pPhysxManager == nullptr)
		m_pPhysxManager = new PhysXManager();

    return m_pPhysxManager;
}

void PhysXManager::Initialize()
{
	//WIN32 Allocator Implementation
	m_pDefaultAllocator = new PhysxAllocator();
	m_pDefaultErrorCallback = new PhysxErrorCallback();

	//Create Foundation
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *m_pDefaultAllocator, *m_pDefaultErrorCallback);
	assert(m_pFoundation != nullptr);

#ifdef _DEBUG
	m_pPvd = PxCreatePvd(*m_pFoundation);
	if (!m_pPvd)
	{
		Logger::GetInstance()->LogWarning("Physx: PhysX Visual Debugger (PVD) creation failed! (PVD connection will not be possible)");
	}
#endif

	//Create Main Physics Object
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale::PxTolerancesScale(), true, m_pPvd);
	assert(m_pPhysics != nullptr);

	PxInitExtensions(*m_pPhysics, m_pPvd);

	//Create Default CpuDispatcher (2 threads)
	m_pDefaultCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

	//Try to connect with the PVD
	ToggleVisualDebuggerConnection();
}

PhysXManager::PhysXManager()
{
	Initialize();
}

PhysXManager::~PhysXManager()
{
	if (m_pPhysics)
	{
		PxCloseExtensions();
	}

	if (m_pPvd && m_pPvd->getTransport())
	{
		if (m_pPvd->isConnected())
		{
			m_pPvd->getTransport()->flush();
			m_pPvd->getTransport()->disconnect();
			m_pPvd->disconnect();
		}

		m_pPvd->getTransport()->release();
	}

	if (m_pPhysics != nullptr)
	{
		m_pPhysics->release();
		m_pPhysics = nullptr;
	}

	if (m_pPvd != nullptr)
	{
		m_pPvd->release();
		m_pPvd = nullptr;
	}

	if (m_pCudaContextManager != nullptr)
	{
		m_pCudaContextManager->release();
		m_pCudaContextManager = nullptr;
	}

	if (m_pDefaultCpuDispatcher != nullptr)
	{
		m_pDefaultCpuDispatcher->release();
		m_pDefaultCpuDispatcher = nullptr;
	}

	if (m_pFoundation != nullptr)
	{
		m_pFoundation->release();
		m_pFoundation = nullptr;
	}

	if (m_pDefaultAllocator)
	{
		delete m_pDefaultAllocator;
		m_pDefaultAllocator = nullptr;
	}

	if (m_pDefaultErrorCallback)
	{
		delete m_pDefaultErrorCallback;
		m_pDefaultErrorCallback = nullptr;
	}
}

bool PhysXManager::ToggleVisualDebuggerConnection() const
{
	if (!m_pPhysics || !m_pPvd) return false;

	if (m_pPvd->isConnected())
	{
		//DISCONNECT
		m_pPvd->disconnect();
		return false;
	}

	//CONNECT
	physx::PxPvdTransport* pTransport = m_pPvd->getTransport();
	if (!pTransport)
	{
		pTransport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, PVD_PORT, 10);
	}

	if (m_pPvd->connect(*pTransport, physx::PxPvdInstrumentationFlag::eALL))
	{
		return true;
	}

	return false;
}