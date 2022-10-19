#include "RigidbodyComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "Scene.h"

#include "PhysxProxy.h"
#include "PhysxHelper.h"
#include "Logger.h"

RigidBodyComponent::RigidBodyComponent(bool isStatic)
	: m_IsStatic(isStatic)
	, m_CollisionGroups(physx::PxFilterData(static_cast<UINT32>(CollisionGroup::Group0), 0, 0, 1))
{
}

RigidBodyComponent::~RigidBodyComponent()
{
	if (m_pActor != nullptr)
	{
		if (!m_pActor->getScene()) m_pActor->release();
	}
}

void RigidBodyComponent::CreateActor()
{
	const auto pPhysX = PhysXManager::GetInstance()->GetPhysics();
	const auto pPhysxScene = m_pGameobject->GetScene()->GetPhysXProxy()->GetPhysxScene();
	const auto pTransform = m_pGameobject->GetTransform();

	if (m_IsStatic)
		m_pActor = pPhysX->createRigidStatic(physx::PxTransform(PhysxHelper::ToPxVec3(pTransform->GetPosition()), PhysxHelper::ToPxQuat(pTransform->GetRotation())));
	else
	{
		m_pActor = pPhysX->createRigidDynamic(physx::PxTransform(PhysxHelper::ToPxVec3(pTransform->GetPosition()), PhysxHelper::ToPxQuat(pTransform->GetRotation())));
		m_pActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, m_IsKinematic);

		if (m_InitialConstraints != RigidBodyConstraint::None)
		{
			SetConstraint(RigidBodyConstraint::All, true);
			SetConstraint(m_InitialConstraints, false);
		}
	}

	pPhysxScene->addActor(*m_pActor);
	m_pActor->userData = this;
}

UINT RigidBodyComponent::_AddCollider(const physx::PxGeometry& geometry, const physx::PxMaterial& material, bool isTrigger, const physx::PxTransform& localPose, UINT colliderId)
{
	assert(m_pActor != nullptr); //This version of AddCollider can only be called if m_pActor exists

	physx::PxShape* pShape = physx::PxRigidActorExt::createExclusiveShape(*m_pActor, geometry, material);
	pShape->setLocalPose(localPose);
	pShape->setSimulationFilterData(m_CollisionGroups);
	pShape->setQueryFilterData(m_CollisionGroups);

	if (colliderId == UINT_MAX)
	{
		colliderId = UINT(m_ColliderCreationInfos.size() + m_Colliders.size());
	}

	m_Colliders.emplace_back(colliderId, this, pShape);
	m_Colliders.back().SetTrigger(isTrigger);

	return colliderId;
}

void RigidBodyComponent::RemoveCollider(const ColliderInfo & colliderInfo)
{
	if (colliderInfo.GetOwner() != this)
	{
		Logger::GetInstance()->LogWarning("[RigidBodyComponent] Failed to remove the given collider. (Incorrect owner)");
		return;
	}

	auto it = std::find(m_Colliders.begin(), m_Colliders.end(), colliderInfo);
	if (it == m_Colliders.end())
	{
		Logger::GetInstance()->LogWarning("[RigidBodyComponent] Failed to remove the given collider. (Collider not found on owner)");
		return;
	}

	physx::PxShape* pShape = colliderInfo.GetShape();
	m_pActor->detachShape(*pShape);
	pShape->release();
	m_Colliders.erase(it);
}

void RigidBodyComponent::Start()
{
	if (!m_pActor)
		CreateActor();

	for (const auto& cci : m_ColliderCreationInfos)
	{
		_AddCollider(*cci.pGeometry, *cci.pMaterial, cci.isTrigger, cci.localPose);
	}
	m_ColliderCreationInfos.clear();
}

void RigidBodyComponent::SetKinematic(bool isKinematic)
{
	if (m_IsStatic && isKinematic) {
		Logger::GetInstance()->LogWarning("A static RigidBody can never be kinematic");
		return;
	}

	m_IsKinematic = isKinematic;

	if (m_pActor != nullptr && !m_IsStatic)
	{
		m_pActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, m_IsKinematic);
	}
}

void RigidBodyComponent::PutToSleep() const
{
}

void RigidBodyComponent::WakeUp() const
{
}

void RigidBodyComponent::AddForce(const DirectX::XMFLOAT3& force, physx::PxForceMode::Enum mode, bool autowake) const
{
	if (m_pActor != nullptr && !m_IsStatic && !m_IsKinematic)
		m_pActor->is<physx::PxRigidDynamic>()->addForce(PhysxHelper::ToPxVec3(force), mode, autowake);
	else
		Logger::GetInstance()->LogWarning("Cannot apply a force on a static or kinematic actor");
}

void RigidBodyComponent::AddTorque(const DirectX::XMFLOAT3& torque, physx::PxForceMode::Enum mode, bool autowake) const
{
	if (m_pActor != nullptr && !m_IsStatic && !m_IsKinematic)
		m_pActor->is<physx::PxRigidDynamic>()->addTorque(PhysxHelper::ToPxVec3(torque), mode, autowake);
	else
		Logger::GetInstance()->LogWarning("Cannot apply torque on a static or kinematic actor");
}

void RigidBodyComponent::ClearForce(physx::PxForceMode::Enum mode) const
{
	if (m_pActor != nullptr && !m_IsStatic && !m_IsKinematic)
		m_pActor->is<physx::PxRigidDynamic>()->clearForce(mode);
	else
		Logger::GetInstance()->LogWarning("Cannot clear force on a static or kinematic actor");
}

void RigidBodyComponent::ClearTorque(physx::PxForceMode::Enum mode) const
{
	if (m_pActor != nullptr && !m_IsStatic && !m_IsKinematic)
		m_pActor->is<physx::PxRigidDynamic>()->clearTorque(mode);
	else
		Logger::GetInstance()->LogWarning("Cannot clear torque on a static or kinematic actor");
}

void RigidBodyComponent::SetConstraint(RigidBodyConstraint flag, bool enable)
{
	if (m_IsStatic || m_IsKinematic)
	{
		Logger::GetInstance()->LogWarning("Can't constrain a static or kinematic actor!");
		return;
	}

	if (m_pActor)
	{
		//TODO: Refactor!!
		const auto pRigidBody = m_pActor->is<physx::PxRigidDynamic>();
		if (isSet(flag, RigidBodyConstraint::RotX))
		{
			pRigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, !enable);
		}
		if (isSet(flag, RigidBodyConstraint::RotY))
		{
			pRigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, !enable);
		}
		if (isSet(flag, RigidBodyConstraint::RotZ))
		{
			pRigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, !enable);
		}
		if (isSet(flag, RigidBodyConstraint::TransX))
		{
			pRigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, !enable);
		}
		if (isSet(flag, RigidBodyConstraint::TransY))
		{
			pRigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, !enable);
		}
		if (isSet(flag, RigidBodyConstraint::TransZ))
		{
			pRigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, !enable);
		}
	}
	else
	{
		if (!enable) m_InitialConstraints |= flag;
		else m_InitialConstraints &= ~flag;
	}
}
