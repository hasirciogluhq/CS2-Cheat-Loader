#pragma once
#include <initializer_list>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/primitives/layout.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Breadcrumb>. Returns the clicked (non-last) index or -1.
inline int breadcrumb(const char* id, std::initializer_list<const char*> items) {
    using namespace theme;
    int clicked = -1, i = 0;
    const int n = static_cast<int>(items.size());
    ImGui::PushID(id);
    ImGui::PushFont(fonts::regular, m.font_sm + 0.5f);
    for (const char* it : items) {
        const bool last = i == n - 1;
        const ImVec2 ts = ImGui::CalcTextSize(it);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::PushID(i);
        ImGui::InvisibleButton("##b", ts);
        ImGui::PopID();
        const bool hov = ImGui::IsItemHovered() && !last;
        if (ImGui::IsItemClicked() && !last) clicked = i;
        ImGui::GetWindowDrawList()->AddText(pos, ImGui::GetColorU32(last || hov ? c.foreground : c.muted_foreground), it);
        if (!last) {
            ImGui::SameLine(0.f, m.space_2);
            ImGui::PushStyleColor(ImGuiCol_Text, c.muted_foreground);
            ImGui::TextUnformatted("/");
            ImGui::PopStyleColor();
            ImGui::SameLine(0.f, m.space_2);
        }
        ++i;
    }
    ImGui::PopFont();
    ImGui::PopID();
    return clicked;
}

}  // namespace ui::w
