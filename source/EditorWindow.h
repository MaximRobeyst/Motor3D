#pragma once
#include <string>
#include <imgui.h>

class EditorWindow
{
public:
	EditorWindow(std::string title, ImVec2 size);
	virtual ~EditorWindow() = default;

	void RootDraw();

	virtual void Initialize() {};
	virtual void Cleanup() {};

	std::string GetTitle() const;

protected:
	virtual void Draw() {};

	std::string m_Name;
	ImVec2 m_Size;
};

