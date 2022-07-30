#include "ImGuiHelpers.h"

bool ImGui::Input(const char* label, int& input)
{
    return ImGui::InputInt(label, &input);
}

bool ImGui::Input(const char* label, float& input)
{
    return ImGui::InputFloat(label, &input);
}

bool ImGui::Input(const char* label, bool& input)
{
    return ImGui::Checkbox(label, &input);
}

bool ImGui::Input(const char* label, DirectX::XMFLOAT2& input)
{
    return InputFloat2(label, &input.x);
}

bool ImGui::Input(const char* label, DirectX::XMFLOAT3& input)
{
    return InputFloat3(label, &input.x);
}

bool ImGui::Input(const char* label, DirectX::XMFLOAT4& input)
{
    return InputFloat4(label, &input.x);
}
