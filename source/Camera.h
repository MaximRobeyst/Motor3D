#pragma once
#pragma warning (push, 0)
#include <wtypes.h>
#pragma warning(pop)

#include <DirectXMath.h>

class Camera 
{
public:
	Camera(float FOV, float aspectRatio, float cfar = 100.f, float cnear = 0.1f);
	Camera(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& forward, float FOV, float aspectRatio, float cfar = 100.f, float cnear = 0.1f);
	~Camera() = default;

	// CopCamera/move constructors and assignment operators
	Camera(const Camera& other) = delete;
	Camera(Camera&& other) noexcept = delete;
	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(Camera&& other)	noexcept = delete;

	// Member functions						
	DirectX::XMFLOAT4X4 GetView() const { return m_View; };
	DirectX::XMFLOAT4X4 GetViewInv() const { return m_ViewInv; };
	DirectX::XMFLOAT4X4 GetProjectionMatrix() const { return m_ProjectionMatrix; };
	DirectX::XMFLOAT4X4 GetViewProjection() const { return m_ViewProjection; };
	DirectX::XMFLOAT4X4 GetViewProjectionMatrix() const { return m_ViewProjection; };

	DirectX::XMFLOAT2 GetAbsoluteRotation() const { return m_AbsoluteRotation; }

	void UpdateCamera();
	void KeyDown(WPARAM wparam);
	void KeyUp(WPARAM wparam);

	void CameraGui();
	void CreateProjectionMatrix(float width, float height);
	void CreateProjectionMatrix(float width, float height, float FOV, float cnear, float cfar);

protected:
	// Private member functions		
	virtual void UpdateMatrix();

	// Datamembers		
	bool m_LeftMouseButtonPressed;

	const float m_KeyboardMoveSensitivity{ 15.f };
	const float m_KeyboardMoveMultiplier{ 10.f };
	const float m_MouseRotationSensitivity{ 2.f };
	const float m_MouseMoveSensitivity{ 10 };
	float m_FOV;
	float m_AspectRatio{};
	float m_Far{};
	float m_Near{};

	DirectX::XMFLOAT3 m_InputVel{};

	DirectX::XMFLOAT2 m_AbsoluteRotation{}; //Pitch(x) & Yaw(y) only
	DirectX::XMFLOAT3 m_RelativeTranslation{};

	DirectX::XMFLOAT3 m_Position{};
	DirectX::XMFLOAT3 m_Forward{};
	DirectX::XMFLOAT3 m_Up{};

	DirectX::XMFLOAT4X4 m_View{};
	DirectX::XMFLOAT4X4 m_ViewInv{};
	DirectX::XMFLOAT4X4 m_ProjectionMatrix{};
	DirectX::XMFLOAT4X4 m_ViewProjection{};
	DirectX::XMFLOAT4X4 m_ViewProjectionInv{};

	DirectX::XMFLOAT2 m_PrevMousePos{};
};

