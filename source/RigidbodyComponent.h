#pragma once
#include "Component.h"
#include "PhysXManager.h"
#include "EnumHelpers.h"

// Physx framework is taken from the overlord engine
class RigidBodyComponent;

enum class RigidBodyConstraint
{
	None = 0,
	RotX = 1 << 0,
	RotY = 1 << 1,
	RotZ = 1 << 2,
	TransX = 1 << 3,
	TransY = 1 << 4,
	TransZ = 1 << 5,

	AllRot = RotX | RotY | RotZ,
	AllTrans = TransX | TransY | TransZ,
	All = AllRot | AllTrans
};
template<>
struct EnableBitMaskOperators<RigidBodyConstraint>
{
static const bool enable = true; 
};

enum class CollisionGroup : UINT32
{
	None = 0,
	Group0 = (1 << 0),
	Group1 = (1 << 1),
	Group2 = (1 << 2),
	Group3 = (1 << 3),
	Group4 = (1 << 4),
	Group5 = (1 << 5),
	Group6 = (1 << 6),
	Group7 = (1 << 7),
	Group8 = (1 << 8),
	Group9 = (1 << 9)
};

class ColliderInfo
{
public:
	ColliderInfo(UINT Id, RigidBodyComponent* pOwner, physx::PxShape* pShape) :
		m_ColliderId{ Id },
		m_pOwner{ pOwner },
		m_pShape(pShape){}

	physx::PxShape* GetShape() const { return m_pShape; }
	bool IsTrigger() const { return m_IsTrigger; }
	const std::wstring& GetTag() const { return m_Tag; }
	RigidBodyComponent* GetOwner() const { return m_pOwner; }
	UINT GetColliderId() const { return m_ColliderId; }

	void SetTag(const std::wstring& tag) { m_Tag = tag; }
	void SetTrigger(bool isTrigger)
	{
		if (m_pShape && m_IsTrigger != isTrigger)
		{
			m_IsTrigger = isTrigger;
			m_pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_IsTrigger);
			m_pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_IsTrigger);
		}
	}

	static ColliderInfo INVALID;

private:
	RigidBodyComponent* m_pOwner{};
	physx::PxShape* m_pShape{};
	bool m_IsTrigger{};
	std::wstring m_Tag{};
	UINT m_ColliderId{ UINT_MAX };
};


inline bool operator==(const ColliderInfo& lhs, const ColliderInfo& rhs)
{
	return lhs.GetOwner() == rhs.GetOwner() && lhs.GetShape() == rhs.GetShape();
}

class RigidBodyComponent final : public IComponent
{
public:
	RigidBodyComponent(bool isStatic = false);
	~RigidBodyComponent() override;

	RigidBodyComponent(const RigidBodyComponent& other) = delete;
	RigidBodyComponent(RigidBodyComponent&& other) noexcept = delete;
	RigidBodyComponent& operator=(const RigidBodyComponent& other) = delete;
	RigidBodyComponent& operator=(RigidBodyComponent&& other) noexcept = delete;

	void Start() override;

	bool IsStatic()  const { return m_IsStatic; }
	bool IsDynamic()  const { return !m_IsStatic; }
	bool IsKinematic() const { return m_IsKinematic; }
	void SetKinematic(bool isKinematic);
	void PutToSleep() const;
	void WakeUp() const;

	physx::PxRigidActor* GetPxRigidActor() const { return m_pActor; };
	void AddForce(const DirectX::XMFLOAT3& force, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE, bool autowake = true) const;
	void AddTorque(const DirectX::XMFLOAT3& torque, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE, bool autowake = true) const;
	void ClearForce(physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE) const;
	void ClearTorque(physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE) const;

	void SetConstraint(RigidBodyConstraint flag, bool enable);
	void SetCollisionGroup(CollisionGroup group);
	void SetCollisionIgnoreGroups(CollisionGroup ignoreGroups);
	void SetDensity(float density) const;

	//Internal Use (use Transform Component for Transformations)
	void Translate(const DirectX::XMFLOAT3& position) const;
	void Rotate(const DirectX::XMFLOAT4& rotation) const;
	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMFLOAT4 GetRotation() const;

	//Shapes (Colliders)
	template<typename T>
	UINT AddCollider(const T& geometry, const physx::PxMaterial& material, bool isTrigger = false, const physx::PxTransform& localPose = physx::PxTransform(physx::PxIdentity));
	void RemoveCollider(const ColliderInfo& colliderInfo);
	void RemoveColliders();
	const std::vector<ColliderInfo>& GetColliders() const { return m_Colliders; }
	ColliderInfo& GetCollider(UINT colliderId);


private:
	//Temporary Storage data
	struct ColliderCreationInfo
	{
		UINT colliderId{};
		const std::shared_ptr<physx::PxGeometry> pGeometry{};
		const physx::PxMaterial* pMaterial{};
		physx::PxTransform localPose{};
		bool isTrigger{};
	};
	std::vector<ColliderCreationInfo> m_ColliderCreationInfos{};

	physx::PxRigidActor* m_pActor{};
	std::vector<ColliderInfo> m_Colliders{};

	bool m_IsStatic{};
	bool m_IsKinematic{};

	physx::PxFilterData m_CollisionGroups{};

	RigidBodyConstraint m_InitialConstraints{};

	void CreateActor();
	UINT _AddCollider(const physx::PxGeometry& geometry, const physx::PxMaterial& material, bool isTrigger = false, const physx::PxTransform& localPose = physx::PxTransform(physx::PxIdentity), UINT colliderId = UINT_MAX);
};

template <typename T>
UINT RigidBodyComponent::AddCollider(const T& geometry, const physx::PxMaterial& material, bool isTrigger, const physx::PxTransform& localPose)
{
	if (!m_pActor)
	{
		const UINT colliderId = UINT(m_ColliderCreationInfos.size());
		const std::shared_ptr<physx::PxGeometry> pGeom(new T{ geometry }); //Prevent PxGeometry Object Slicing...

		m_ColliderCreationInfos.push_back({ colliderId, pGeom, &material, localPose, isTrigger });
		return colliderId;
	}

	return _AddCollider(geometry, material, isTrigger, localPose);
}


