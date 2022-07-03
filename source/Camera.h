#pragma once
#pragma warning (push, 0)
#include <wtypes.h>
#pragma warning(pop)

#include <DirectXMath.h>


class Camera final
{
public:
	Camera(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& forward, float FOV, float aspectRatio, float cfar = 100.f, float cnear = 0.1f);
	~Camera() = default;

	// CopCamera/move constructors and assignment operators
	Camera(const Camera& other) = delete;
	Camera(Camera&& other) noexcept = delete;
	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(Camera&& other)	noexcept = delete;

	// Member functions						
	DirectX::XMFLOAT4X4 GetViewMatrix() const;
	DirectX::XMFLOAT4X4 GetWorldMatrix() const;
	DirectX::XMFLOAT4X4 GetProjectionMatrix() const;

	void Update(float elapsedSec);
	void KeyDown(WPARAM wparam);
	void KeyUp(WPARAM wparam);

private:
	// Private member functions		
	void UpdateMatrix();
	void MakeProjectionMatrix();

	// Datamembers				
	float m_FOV;
	bool m_LeftMouseButtonPressed;

	const float m_KeyboardMoveSensitivity{ 15.f };
	const float m_KeyboardMoveMultiplier{ 10.f };
	const float m_MouseRotationSensitivity{ 1.f };
	const float m_MouseMoveSensitivity{ 200.f };
	const float m_AspectRatio{};
	const float m_Far{};
	const float m_Near{};

	DirectX::XMFLOAT3 m_InputVel{};

	DirectX::XMFLOAT2 m_AbsoluteRotation{}; //Pitch(x) & Yaw(y) only
	DirectX::XMFLOAT3 m_RelativeTranslation{};

	DirectX::XMFLOAT3 m_Position{};
	DirectX::XMFLOAT3 m_Forward{};

	DirectX::XMFLOAT4X4 m_WorldToView{};
	DirectX::XMFLOAT4X4 m_ViewToWorld{};
	DirectX::XMFLOAT4X4 m_ProjectionMatrix{};
	DirectX::XMFLOAT4X4 m_WorldViewProjectionMatrix{};

	DirectX::XMFLOAT2 m_PrevMousePos{};
};

