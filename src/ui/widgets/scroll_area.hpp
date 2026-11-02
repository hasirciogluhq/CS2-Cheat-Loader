#pragma once
#include "imgui.h"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <ScrollArea>: borderless scrolling region (slim scrollbar). size {0,0} = fill remaining.
// Always call scroll_area_end().
inline bool scroll_area_begin(const char* id, ImVec2 size = {0.f, 0.f}) {
    using namespace theme;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0, 0, 0, 0});
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, with_alpha(c.input, 0.35f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, with_alpha(c.input, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, with_alpha(c.input, 0.8f));
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 6.f);
    return ImGui::BeginChild(id, size, ImGuiChildFlags_None, ImGuiWindowFlags_None);
}
inline void scroll_area_end() {
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
}

}  // namespace ui::w
