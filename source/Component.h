#pragma once
#pragma warning (push, 0)
#include "DataTypes.h"
#pragma warning(pop)

#include <vector>
#include <DirectXMath.h>
#include <wtypes.h>

#include "Camera.h"
#include "GameObject.h"

#include "Serialization.h"

class Mesh;
class TransformComponent;

class IComponent
{
public:
	IComponent();
	virtual ~IComponent();


	virtual void Start() {};
	virtual void Render();
	virtual void Update();

	virtual void RegisterMembers() {};

	virtual void RenderGUI() { };

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>& ) {};
	virtual void Deserialize(const rapidjson::Value&) {};

	virtual void OnTriggerEnter(GameObject*) {}
	virtual void OnTriggerExit(GameObject*) {}

	void SetGameobject(GameObject* pGameobject);
	GameObject* GetGameObject() const;

protected:
	GameObject* m_pGameobject{};
};

class CameraComponent : public IComponent, public Camera
{
public:
	CameraComponent(float FOV = 60.f, float aspectRatio = 1.0f, float cfar = 100.f, float cnear = 0.1f);
	~CameraComponent() = default;

	// CopCamera/move constructors and assignment operators
	CameraComponent(const CameraComponent& other) = delete;
	CameraComponent(CameraComponent&& other) noexcept = delete;
	CameraComponent& operator=(const CameraComponent& other) = delete;
	CameraComponent& operator=(CameraComponent&& other)	noexcept = delete;

	void Start() override;
	void Update() override;
	void Render() override;

	void KeyDown(WPARAM wparam);
	void KeyUp(WPARAM wparam);

	void RenderGUI() override;

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>& writer);
	virtual void Deserialize(const rapidjson::Value&);

protected:
	void UpdateMatrix() override;

	bool m_LeftMouseButtonPressed;

	const float m_KeyboardMoveSensitivity{ 15.f };
	const float m_KeyboardMoveMultiplier{ 10.f };
	const float m_MouseRotationSensitivity{ 1.f };
	const float m_MouseMoveSensitivity{ 200.f };

	TransformComponent* m_pTransformComponent{};

	DirectX::XMFLOAT3 m_InputVel{};

	DirectX::XMFLOAT2 m_AbsoluteRotation{}; //Pitch(x) & Yaw(y) only
	DirectX::XMFLOAT3 m_RelativeTranslation{};

	DirectX::XMFLOAT2 m_PrevMousePos{};
};

class Rotator : public IComponent
{
public:
	Rotator(float rotationSpeed = 45.f, DirectX::XMFLOAT3 axis = DirectX::XMFLOAT3{ 0, 1, 0 } );

	void Update() override;

	void RenderGUI() override;

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>& writer);
	virtual void Deserialize(const rapidjson::Value&);

private:
	bool m_Enabled{false};
	float m_Rotation{};
	float m_RotationSpeed{};
	DirectX::XMFLOAT3 m_Axis{};

};
