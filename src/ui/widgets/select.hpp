#pragma once
#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Select>. items: array of C strings. width: <0 fill, >0 fixed.
inline bool select(const char* id, int& index, const char* const* items, int count, float width = -1.f) {
    using namespace theme;
    ImGui::PushFont(fonts::regular, m.font_base);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, with_alpha(c.input, 0.08f));
    ImGui::PushStyleColor(ImGuiCol_Border, c.input);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, c.popover);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, m.radius);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, m.radius);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {m.space_1, m.space_1});
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {m.space_3 - 2.f, (m.control_md - ImGui::GetFontSize()) * 0.5f});
    ImGui::SetNextItemWidth(width < 0.f ? -FLT_MIN : width);
    bool changed = false;
    if (ImGui::BeginCombo(id, items[index])) {
        for (int i = 0; i < count; ++i) {
            if (ImGui::Selectable(items[i], i == index)) { changed = index != i; index = i; }
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleVar(5);
    ImGui::PopStyleColor(3);
    ImGui::PopFont();
    return changed;
}

}  // namespace ui::w
