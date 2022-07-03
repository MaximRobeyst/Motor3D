#pragma once
#pragma warning (push, 0)
#include "DataTypes.h"
#pragma warning(pop)

#include <vector>
#include <DirectXMath.h>
#include <wtypes.h>

#include "GameObject.h"
#include "FieldBinder.h"

class Mesh;
class Camera;

class IComponent
{
public:
	IComponent(uint8_t componentID);
	virtual ~IComponent() = default;

	virtual void Start() {};
	virtual void Render(Camera* pCamera, GameObject* pGameobject);
	virtual void Update(float dt);

	virtual void RenderGUI() {};

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>& ) {};
	virtual void Deserialize(const rapidjson::Value&) {};

	void SetGameobject(GameObject* pGameobject);

protected:
	uint8_t m_ComponentID;
	GameObject* m_pGameobject{};
};

class TransformComponent : public IComponent
{
public:
	TransformComponent(DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3{}, DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3{}, DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3{ 1,1,1 });

	void Update(float dt) override;

	void RenderGUI() override;

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>& writer);
	virtual void Deserialize(const rapidjson::Value&);

	DirectX::XMFLOAT4X4 GetWorldMatrix();
	
	DirectX::XMFLOAT3 GetPosition() const;
	void SetPosition(DirectX::XMFLOAT3 position);

	DirectX::XMFLOAT3 GetRotation() const;
	void SetRotation(DirectX::XMFLOAT3 rotation);

	void SetParent(TransformComponent* pTransformComponent);

	DirectX::XMFLOAT3& GetForward();
	DirectX::XMFLOAT3& GetRight();
	DirectX::XMFLOAT3& GetUp();

private:
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Rotation;
	DirectX::XMFLOAT3 m_Scale;

	DirectX::XMFLOAT4X4 m_WorldMatrix;

	DirectX::XMFLOAT3 m_Forward;
	DirectX::XMFLOAT3 m_Right;
	DirectX::XMFLOAT3 m_Up;
};

class CameraComponent : public IComponent
{
public:
	CameraComponent(float FOV = 60.f, float aspectRatio = 1.0f, float cfar = 100.f, float cnear = 0.1f);
	~CameraComponent() = default;

	// CopCamera/move constructors and assignment operators
	CameraComponent(const CameraComponent& other) = delete;
	CameraComponent(CameraComponent&& other) noexcept = delete;
	CameraComponent& operator=(const CameraComponent& other) = delete;
	CameraComponent& operator=(CameraComponent&& other)	noexcept = delete;

	// Member functions						
	DirectX::XMFLOAT4X4 GetView() const { return m_View; };
	DirectX::XMFLOAT4X4 GetViewInv() const { return m_ViewInv;  };
	DirectX::XMFLOAT4X4 GetProjectionMatrix() const { return m_ProjectionMatrix; };
	DirectX::XMFLOAT4X4 GetViewProjection() const { return m_ViewProjection; };
	DirectX::XMFLOAT4X4 GetViewProjectionMatrix() const { return m_ViewProjection; };

	void Start() override;
	void Update(float elapsedSec) override;
	void KeyDown(WPARAM wparam);
	void KeyUp(WPARAM wparam);

private:
	// Datamembers				
	float m_FOV{};
	float m_AspectRatio{};
	float m_Far{};
	float m_Near{};

	bool m_LeftMouseButtonPressed;

	const float m_KeyboardMoveSensitivity{ 15.f };
	const float m_KeyboardMoveMultiplier{ 10.f };
	const float m_MouseRotationSensitivity{ 1.f };
	const float m_MouseMoveSensitivity{ 200.f };

	TransformComponent* m_pTransformComponent{};

	DirectX::XMFLOAT3 m_InputVel{};

	DirectX::XMFLOAT2 m_AbsoluteRotation{}; //Pitch(x) & Yaw(y) only
	DirectX::XMFLOAT3 m_RelativeTranslation{};

	DirectX::XMFLOAT4X4 m_View{};
	DirectX::XMFLOAT4X4 m_ViewInv{};
	DirectX::XMFLOAT4X4 m_ProjectionMatrix{};
	DirectX::XMFLOAT4X4 m_ViewProjection{};
	DirectX::XMFLOAT4X4 m_ViewProjectionInv{};

	DirectX::XMFLOAT2 m_PrevMousePos{};
};

class RigidBodyComponent : public IComponent
{
public:
	RigidBodyComponent(DirectX::XMFLOAT3 velocity = DirectX::XMFLOAT3{}, DirectX::XMFLOAT3 acceleration = DirectX::XMFLOAT3{}, DirectX::XMFLOAT3 gravity = DirectX::XMFLOAT3{});
	void Update(float dt) override;

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>&);
	virtual void Deserialize(const rapidjson::Value&);

	void UpdateTransform(TransformComponent* tc);	// temp function

private:
	DirectX::XMFLOAT3 m_Velocity{};
	DirectX::XMFLOAT3 m_Acceleration{};
	DirectX::XMFLOAT3 m_Gravity{};
};

class MeshComponent : public IComponent
{
public:
	MeshComponent(Mesh* pMesh = nullptr);
	~MeshComponent();

	void Render(Camera* pCamera, GameObject* pGameobject) override;
	void Update(float dt) override;

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>&);
	virtual void Deserialize(const rapidjson::Value&);
private:
	Mesh* m_pMesh{};
};

class Rotator : public IComponent
{
public:
	Rotator(float rotationSpeed = 45.f, DirectX::XMFLOAT3 axis = DirectX::XMFLOAT3{ 0, 1, 0 } );

	void Update(float dt) override;

	void RenderGUI() override;

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>& writer);
	virtual void Deserialize(const rapidjson::Value&);

private:
	bool m_Enabled{false};
	float m_Rotation{};
	float m_RotationSpeed{};
	DirectX::XMFLOAT3 m_Axis{};

};
