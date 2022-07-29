#pragma once
#include <imgui.h>
#include <imconfig.h>

#include "EditorWindow.h"

class Logger;
//https://github.com/ocornut/imgui/issues/300
class LogWindow : public EditorWindow
{
public:
    LogWindow(std::string title = "Logger", ImVec2 size = ImVec2{500, 400});

    void Clear();
    void Draw() override;
    void Cleanup() override;

    void AddLog(const char* fmt, ...) ;

private:
    Logger* m_pLogger{};

    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset
    bool                ScrollToBottom;
};

