#pragma once
#pragma warning (push, 0)
#include "DataTypes.h"
#pragma warning(pop)


class Mesh;
class Camera;
class GameObject;

class IComponent
{
public:
	IComponent(uint8_t componentID);
	virtual ~IComponent() = default;

	virtual void Render(Camera* pCamera, GameObject* pGameobject);
	virtual void Update(float dt, GameObject* pGameobject);
protected:
	uint8_t m_ComponentID;
};

class TransformComponent : public IComponent
{
public:
	TransformComponent(FVector3 pos = FVector3{}, FVector3 rotation = FVector3{}, FVector3 scale = FVector3{ 1,1,1 });

	void Update(float dt);
	FMatrix4 GetWorldMatrix() const;
	
	FVector3 GetPosition() const;
	void SetPosition(FVector3 position);

	FVector3 GetRotation() const;
	void SetRotation(FVector3 rotation);
private:
	FVector3 m_Position;
	FVector3 m_Rotation;
	FVector3 m_Scale;
};

class RigidBodyComponent : public IComponent
{
public:
	RigidBodyComponent(FVector3 velocity = FVector3{}, FVector3 acceleration = FVector3{}, FVector3 gravity = FVector3{});
	void Update(float dt, GameObject* pGameobject) override;

	void UpdateTransform(TransformComponent* tc);	// temp function
private:
	FVector3 m_Velocity{};
	FVector3 m_Acceleration{};
	FVector3 m_Gravity{};
};

class MeshComponent : public IComponent
{
public:
	MeshComponent(Mesh* pMesh);
	~MeshComponent();

	void Render(Camera* pCamera, GameObject* pGameobject) override;
	void Update(float dt, GameObject* pGameobject) override;
private:
	Mesh* m_pMesh{};
};

class Rotator : public IComponent
{
public:
	Rotator(float rotationSpeed, FVector3 axis);

	void Update(float dt, GameObject* pGameobject) override;
private:
	float m_Rotation{};
	float m_RotationSpeed{};
	FVector3 m_Axis{};

};
