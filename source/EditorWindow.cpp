#include "EditorWindow.h"

EditorWindow::EditorWindow(std::string title, ImVec2 size)
	: m_Name{title}
	, m_Size{size}
{
}

void EditorWindow::RootDraw()
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin(m_Name.c_str());

	Draw();

	ImGui::End();
}

std::string EditorWindow::GetTitle() const
{
	return m_Name;
}
