#pragma once
#include <initializer_list>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/primitives/layout.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Tabs><TabsList>. Draws the tab strip, returns true when selection changed.
//   static int tab = 0;  w::tabs("main", {"Loader", "Settings"}, tab);  if (tab == 0) {...}
inline bool tabs(const char* id, std::initializer_list<const char*> labels, int& selected) {
    using namespace theme;
    const float h = m.control_md + 2.f, pad = 3.f, padx = m.space_4 - 2.f;
    ImGui::PushID(id);
    ImGui::PushFont(fonts::medium, m.font_sm + 0.5f);

    float total = pad * 2.f;
    for (const char* l : labels) total += ImGui::CalcTextSize(l, p::label_end(l), true).x + padx * 2.f;

    const ImVec2 pos = ImGui::GetCursorScreenPos();
    auto* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(pos, {pos.x + total, pos.y + h}, ImGui::GetColorU32(c.muted), m.radius);

    bool changed = false;
    float x = pos.x + pad;
    int i = 0;
    for (const char* l : labels) {
        const float tw = ImGui::CalcTextSize(l, p::label_end(l), true).x;
        const float w = tw + padx * 2.f;
        ImGui::SetCursorScreenPos({x, pos.y + pad});
        ImGui::PushID(i);
        if (ImGui::InvisibleButton("##tab", {w, h - pad * 2.f}) && selected != i) { selected = i; changed = true; }
        const bool hov = ImGui::IsItemHovered();
        ImGui::PopID();
        const bool active = selected == i;
        if (active)
            dl->AddRectFilled({x, pos.y + pad}, {x + w, pos.y + h - pad}, ImGui::GetColorU32(with_alpha(c.input, 0.25f)), m.radius - 2.f);
        const ImVec4 col = active || hov ? c.foreground : c.muted_foreground;
        dl->AddText({x + padx, pos.y + (h - ImGui::GetFontSize()) * 0.5f}, ImGui::GetColorU32(col), l, p::label_end(l));
        x += w;
        ++i;
    }
    ImGui::PopFont();
    ImGui::SetCursorScreenPos(pos);
    ImGui::Dummy({total, h});
    ImGui::PopID();
    return changed;
}

}  // namespace ui::w
