#include "Camera.h"
#include <DirectXMath.h>
#include "MyEngine.h"
#include "Utils.h"
#include <imgui.h>
#include "GameTime.h"

Camera::Camera(float FOV, float aspectRatio, float cfar, float cnear)
	: m_Position{ DirectX::XMFLOAT3{0,0,0 } }
	, m_Forward{ DirectX::XMFLOAT3{0,0,1 } }
	, m_FOV{ FOV }
	, m_AspectRatio{ aspectRatio }
	, m_Far{ cfar }
	, m_Near{ cnear }
{
}

Camera::Camera(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& forward, float FOV, float aspectRatio, float cfar, float cnear)
	: m_Position{ position }
	, m_Forward{  }
	, m_FOV{ FOV }
	, m_AspectRatio{ aspectRatio }
	, m_Far{ cfar }
	, m_Near{ cnear }
{
	DirectX::XMStoreFloat3(&m_Forward, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward)));
	DirectX::XMStoreFloat3(&m_Up, DirectX::XMVectorSet(0, 1, 0, 0));

	RECT windowRect;
	GetWindowRect(MyEngine::GetSingleton()->GetWindowHandle(), &windowRect);
	m_PrevMousePos = DirectX::XMFLOAT2{ static_cast<float>(windowRect.left + ((windowRect.right - windowRect.left) / 2)), static_cast<float>(windowRect.bottom + ((windowRect.top - windowRect.bottom) / 2)) };

	UpdateMatrix();
}

void Camera::UpdateCamera()
{
	float elapsedSec = GameTime::GetInstance().GetElapsed();

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
		m_AbsoluteRotation.x -= Clamp((m_PrevMousePos.y - y), -1.f, 1.f) * m_MouseRotationSensitivity * elapsedSec;
		m_AbsoluteRotation.y -= Clamp((m_PrevMousePos.x - x), -1.f, 1.f) * m_MouseRotationSensitivity * elapsedSec;
	}
	else
	{
		m_AbsoluteRotation.x = 0;
		m_AbsoluteRotation.y = 0;
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

void Camera::CameraGui()
{
	ImGui::SliderAngle("Field of View", &m_FOV, 30.f);
	if (ImGui::InputFloat("Near", &m_Near))
	{
		UpdateMatrix();
	}
	if (ImGui::InputFloat("Far", &m_Far))
	{
		UpdateMatrix();
	}
}


void Camera::CreateProjectionMatrix(float width, float height)
{
	CreateProjectionMatrix(width, height, m_FOV, m_Near, m_Far);
}

void Camera::CreateProjectionMatrix(float width, float height, float FOV, float cnear, float cfar)
{
	m_FOV = FOV;
	m_AspectRatio = width / height;
	m_Near = cnear;
	m_Far = cfar;

	DirectX::XMMATRIX projection{};
	projection = DirectX::XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, m_Near, m_Far);
	DirectX::XMStoreFloat4x4(&m_ProjectionMatrix, projection);
}

void Camera::UpdateMatrix()
{
	DirectX::XMMATRIX projection{};
	projection = DirectX::XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, m_Near, m_Far);

	auto position = DirectX::XMLoadFloat3(&m_Position);
	auto forward = DirectX::XMLoadFloat3(&m_Forward);
	auto up = m_Up;

	DirectX::XMMATRIX yawRotation = DirectX::XMMatrixRotationY(m_AbsoluteRotation.y * float(TO_RADIANS));
	auto zAxis = DirectX::XMVector3TransformCoord(forward, yawRotation);

	//Calculate RIGHT (xAxis) based on transformed FORWARD
	auto xAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSet( 0.f, 1.f, 0.f, 0.f ), zAxis));

	//FORWARD with PITCH applied (based on xAxis)
	DirectX::XMMATRIX pitchRotation = DirectX::XMMatrixRotationX(m_AbsoluteRotation.x * float(TO_RADIANS));
	zAxis = DirectX::XMVector3TransformCoord(zAxis, pitchRotation);

	//Calculate UP (yAxis)
	auto yAxis = DirectX::XMVector3Cross(zAxis, xAxis);

	using namespace DirectX;

	//Translate based on transformed axis
	position = position + (m_RelativeTranslation.x * xAxis);
	position = position + (m_RelativeTranslation.y * yAxis);
	position = position + (m_RelativeTranslation.z * zAxis);

	DirectX::XMStoreFloat3(&m_Forward, zAxis);
	DirectX::XMStoreFloat3(&m_Position, position);

	const DirectX::XMVECTOR worldPosition = DirectX::XMLoadFloat3(&m_Position);
	const DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&m_Forward);
	const DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

	auto LookAt = DirectX::XMVectorAdd(worldPosition, lookAt);

	const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(worldPosition, LookAt, upVec);
	const DirectX::XMMATRIX viewInv = DirectX::XMMatrixInverse(nullptr, view);
	const DirectX::XMMATRIX viewProjectionInv = DirectX::XMMatrixInverse(nullptr, view * projection);

	DirectX::XMStoreFloat4x4(&m_ProjectionMatrix, projection);
	DirectX::XMStoreFloat4x4(&m_View, view);
	DirectX::XMStoreFloat4x4(&m_ViewInv, viewInv);
	DirectX::XMStoreFloat4x4(&m_ViewProjection, view * projection);
	DirectX::XMStoreFloat4x4(&m_ViewProjectionInv, viewProjectionInv);
}