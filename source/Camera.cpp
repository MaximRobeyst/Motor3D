#include "Camera.h"
#include <DirectXMath.h>
#include "MyEngine.h"
#include "Utils.h"

Camera::Camera(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& forward, float FOV, float aspectRatio, float cfar, float cnear)
	: m_Position{ position }
	, m_Forward{  }
	, m_FOV{ tanf( ToRadians(FOV) / 2.f) }
	, m_AspectRatio{ aspectRatio }
	, m_Far{ cfar }
	, m_Near{ cnear }
{
	DirectX::XMStoreFloat3(&m_Forward, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward)));

	RECT windowRect;
	GetWindowRect(MyEngine::GetSingleton()->GetWindowHandle(), &windowRect);
	m_PrevMousePos = DirectX::XMFLOAT2{ static_cast<float>(windowRect.left + ((windowRect.right - windowRect.left) / 2)), static_cast<float>(windowRect.bottom + ((windowRect.top - windowRect.bottom) / 2)) };

	MakeProjectionMatrix();
	UpdateMatrix();
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
		m_AbsoluteRotation.x -= Clamp((m_PrevMousePos.y-y),-1.f,1.f) * m_MouseRotationSensitivity;
		m_AbsoluteRotation.y -= Clamp((m_PrevMousePos.x-x),-1.f,1.f) * m_MouseRotationSensitivity;
	}

	m_PrevMousePos.x = static_cast<float>(x);
	m_PrevMousePos.y = static_cast<float>(y);

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
	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&m_Forward);
	DirectX::XMMATRIX yawRotation = DirectX::XMMatrixRotationY(m_AbsoluteRotation.y * float(TO_RADIANS));
	DirectX::XMVECTOR zAxis = DirectX::XMVector3Transform(forward, yawRotation);

	DirectX::XMFLOAT3 upVector = DirectX::XMFLOAT3{ 0, 1,0 };
	DirectX::XMVECTOR upVetor = DirectX::XMLoadFloat3(&upVector);

	//Calculate RIGHT (xAxis) based on transformed FORWARD
	DirectX::XMVECTOR xAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(upVetor, zAxis));

	//FORWARD with PITCH applied (based on xAxis)
	DirectX::XMMATRIX pitchRotation = DirectX::XMMatrixRotationZ(m_AbsoluteRotation.x * float(TO_RADIANS));
	zAxis = DirectX::XMVector3Transform(zAxis, pitchRotation);

	//Calculate UP (yAxis)
	DirectX::XMVECTOR yAxis = DirectX::XMVector3Cross(zAxis, xAxis);

	//Translate based on transformed axis
	auto position = DirectX::XMLoadFloat3(&m_Position);
	DirectX::XMVectorAdd(position , DirectX::XMVectorMultiply(DirectX::XMLoadFloat(&m_RelativeTranslation.x) , xAxis));
	DirectX::XMVectorAdd(position , DirectX::XMVectorMultiply(DirectX::XMLoadFloat(&m_RelativeTranslation.y) , yAxis));
	DirectX::XMVectorAdd(position , DirectX::XMVectorMultiply(DirectX::XMLoadFloat(&m_RelativeTranslation.z) , zAxis));
	DirectX::XMStoreFloat3(&m_Position, position);

	auto posfloat4 = DirectX::XMFLOAT4{ m_Position.x, m_Position.y, m_Position.z, 1.f };
	position = DirectX::XMLoadFloat4(&posfloat4);

	//Construct View2World Matrix
	DirectX::XMMATRIX viewToWorld
	{
		xAxis,
		yAxis,
		zAxis,
		position
	};

	//Construct World2View Matrix || viewMatrix
	DirectX::XMVECTOR viewToWorldDeterminant = DirectX::XMMatrixDeterminant(viewToWorld);
	viewToWorld = DirectX::XMMatrixInverse(&viewToWorldDeterminant, viewToWorld);
	DirectX::XMStoreFloat4x4(&m_ViewToWorld, viewToWorld);

	// WorldViewProjectionMatrix = ProjectionMatrix * ViewMatrix * WorldMatrix
	auto worldViewProjectionationMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&m_ProjectionMatrix) , viewToWorld);
	DirectX::XMStoreFloat4x4(&m_WorldViewProjectionMatrix, worldViewProjectionationMatrix);
}

void Camera::MakeProjectionMatrix()
{
	m_ProjectionMatrix = DirectX::XMFLOAT4X4{
		1 / (m_AspectRatio * m_FOV),0,0, 0 ,
		0,1 / m_FOV,0, 0 ,
		0,0, m_Far / (m_Far - m_Near), 1.f,
		0, 0,-(m_Far * m_Near) / (m_Far - m_Near),0 
	};
}