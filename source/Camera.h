#pragma once
#pragma warning (push, 0)
#include "DataTypes.h"
#pragma warning(pop)


class Camera final
{
public:
	Camera(const FVector3& position, const FVector3& forward, float FOV, float aspectRatio, float cfar = 100.f, float cnear = 0.1f);
	~Camera() = default;

	// CopCamera/move constructors and assignment operators
	Camera(const Camera& other) = delete;
	Camera(Camera&& other) noexcept = delete;
	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(Camera&& other)	noexcept = delete;

	// Member functions						
	FMatrix4 GetViewMatrix() const;
	FMatrix4 GetWorldMatrix() const;
	FMatrix4 GetProjectionMatrix() const;

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

	FVector3 m_InputVel{};

	FVector2 m_AbsoluteRotation{}; //Pitch(x) & Yaw(y) only
	FVector3 m_RelativeTranslation{};

	FVector3 m_Position{};
	FVector3 m_Forward{};

	FMatrix4 m_WorldToView{};
	FMatrix4 m_ViewToWorld{};
	FMatrix4 m_ProjectionMatrix{};
	FMatrix4 m_WorldViewProjectionMatrix{};

	IVector2 m_PrevMousePos{};
};

