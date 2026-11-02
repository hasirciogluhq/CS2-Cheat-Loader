#pragma once
#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

namespace ui::w {

enum class Variant { Default, Secondary };

// shadcn <Button>. Returns true on click.
inline bool button(const char* label, Variant v = Variant::Default, ImVec2 size = {0.f, 36.f}) {
    const bool primary = v == Variant::Default;
    ImGui::PushStyleColor(ImGuiCol_Button, primary ? theme::primary : theme::secondary);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, primary ? theme::primary_hover : theme::secondary_hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, primary ? theme::primary_active : theme::secondary_hover);
    ImGui::PushStyleColor(ImGuiCol_Text, primary ? theme::primary_fg : theme::foreground);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, theme::radius);
    ImGui::PushFont(fonts::medium, 14.f);
    const bool clicked = ImGui::Button(label, size);
    ImGui::PopFont();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
    return clicked;
}

}  // namespace ui::w
