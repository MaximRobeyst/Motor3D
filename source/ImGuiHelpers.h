#pragma once
#include <imgui.h>
#include <DirectXMath.h>

namespace ImGui
{
	// Fundamental types
	bool Input(const char* label, int& input);
	bool Input(const char* label, float& input);
	bool Input(const char* label, bool& input);

	// Math types
	bool Input(const char* label, DirectX::XMFLOAT2& input);
	bool Input(const char* label, DirectX::XMFLOAT3& input);
	bool Input(const char* label, DirectX::XMFLOAT4& input);
}


