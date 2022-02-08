#include "Camera.h"

Camera::Camera(const FVector3& position, const FVector3& forward, float FOV, float aspectRatio, float cfar, float cnear)
	: m_Position{ position }
	, m_Forward{ GetNormalized(forward) }
	, m_FOV{ tanf( ToRadians(FOV) / 2.f) }
	, m_AspectRatio{ aspectRatio }
	, m_Far{ cfar }
	, m_Near{ cnear }
{
	RECT windowRect;
	GetWindowRect(MyEngine::GetSingleton()->GetWindowHandle(), &windowRect);
	m_PrevMousePos = IVector2{ windowRect.left + ((windowRect.right - windowRect.left) / 2), windowRect.bottom + ((windowRect.top - windowRect.bottom) / 2) };

	MakeProjectionMatrix();
	UpdateMatrix();
}

FMatrix4 Camera::GetViewMatrix() const
{
	return m_WorldToView;
}

FMatrix4 Camera::GetWorldMatrix() const
{
	return m_ViewToWorld;
}

FMatrix4 Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

void Camera::Update(float elapsedSec)
{
	//const uint8_t* pKeyboardState = SDL_GetKeyboardState(0);
	float keyboardSpeed = (GetKeyState(VK_SHIFT) & 0x80) ? m_KeyboardMoveSensitivity * m_KeyboardMoveMultiplier : m_KeyboardMoveSensitivity;
	m_RelativeTranslation.x = (((GetKeyState(0x44) & 0x80) - (GetKeyState(0x41) & 0x80)) / 128) * keyboardSpeed * elapsedSec;
	m_RelativeTranslation.y = (((GetKeyState(0x45) & 0x80) - (GetKeyState(0x51) & 0x80)) / 128) * keyboardSpeed * elapsedSec;;
	m_RelativeTranslation.z = (((GetKeyState(0x57) & 0x80) - (GetKeyState(0x53) & 0x80)) / 128) * keyboardSpeed * elapsedSec;

	//Mouse Input
	POINT point{};
	GetCursorPos(&point);
	int x = point.x;
	int y = point.y;

	if ((GetKeyState(VK_RBUTTON) & 0x80) != 0)
	{
		m_AbsoluteRotation.x -= Clamp((m_PrevMousePos.y-y),-1,1) * m_MouseRotationSensitivity;
		m_AbsoluteRotation.y -= Clamp((m_PrevMousePos.x-x),-1,1) * m_MouseRotationSensitivity;
	}

	m_PrevMousePos.x = x;
	m_PrevMousePos.y = y;

	//Update LookAt (view2world & world2view matrices)
	//*************
	UpdateMatrix();
}

void Camera::KeyDown(WPARAM wparam)
{
	if (wparam == 0x53) // S-Key
		m_InputVel.z = -1;
	if (wparam == 0x57) // W-Key
		m_InputVel.z = 1;
	if (wparam == 0x41) // A-Key
		m_InputVel.x = -1;
	if (wparam == 0x44) // D-Key
		m_InputVel.x = 1;
}

void Camera::KeyUp(WPARAM wparam)
{
	if (wparam == 0x53 || wparam == 0x57)	// S-Key && W-Key
		m_InputVel.z = 0;
	if (wparam == 0x41 || wparam == 0x44)	// A-Key && D-Key
		m_InputVel.x = 0;
}

void Camera::UpdateMatrix()
{
	//FORWARD (zAxis) with YAW applied
	FMatrix3 yawRotation = MakeRotationY(m_AbsoluteRotation.y * float(TO_RADIANS));
	FVector3 zAxis = yawRotation * m_Forward;

	//Calculate RIGHT (xAxis) based on transformed FORWARD
	FVector3 xAxis = GetNormalized(Cross(FVector3{ 0.f,1.f,0.f }, zAxis));

	//FORWARD with PITCH applied (based on xAxis)
	FMatrix3 pitchRotation = MakeRotation(m_AbsoluteRotation.x * float(TO_RADIANS), xAxis);
	zAxis = pitchRotation * zAxis;

	//Calculate UP (yAxis)
	FVector3 yAxis = Cross(zAxis, xAxis);

	//Translate based on transformed axis
	m_Position += m_RelativeTranslation.x * xAxis;
	m_Position += m_RelativeTranslation.y * yAxis;
	m_Position += m_RelativeTranslation.z * zAxis;

	//Construct View2World Matrix
	m_ViewToWorld =
	{
		FVector4{xAxis},
		FVector4{yAxis},
		FVector4{zAxis},
		FVector4{m_Position.x,m_Position.y,m_Position.z,1.f}
	};

	//Construct World2View Matrix || viewMatrix
	m_WorldToView = Inverse(m_ViewToWorld);

	// WorldViewProjectionMatrix = ProjectionMatrix * ViewMatrix * WorldMatrix
	m_WorldViewProjectionMatrix = m_ProjectionMatrix * m_ViewToWorld;
}

void Camera::MakeProjectionMatrix()
{
	m_ProjectionMatrix = FMatrix4{
		FVector4{ 1 / (m_AspectRatio * m_FOV),0,0, 0 },
		FVector4{ 0,1 / m_FOV,0, 0 },
		FVector4{ 0,0, m_Far / (m_Far - m_Near), 1.f},
		FVector4{ 0, 0,-(m_Far * m_Near) / (m_Far - m_Near),0 }
	};
}