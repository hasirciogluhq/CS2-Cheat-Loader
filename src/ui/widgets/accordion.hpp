#pragma once
#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Accordion>/<Collapsible> item. Open state is kept by ImGui.
//   if (w::accordion("Section")) { ...content... }
inline bool accordion(const char* title, bool default_open = false) {
    using namespace theme;
    ImGuiStorage* st = ImGui::GetStateStorage();
    const ImGuiID id = ImGui::GetID(title);
    bool open = st->GetBool(id, default_open);
    const float h = m.control_md + 2.f;
    const float w = ImGui::GetContentRegionAvail().x;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    if (ImGui::InvisibleButton(title, {w, h})) { open = !open; st->SetBool(id, open); }
    const bool hov = ImGui::IsItemHovered();
    auto* dl = ImGui::GetWindowDrawList();
    dl->AddText(fonts::medium, m.font_base, {pos.x, pos.y + (h - m.font_base) * 0.5f},
                ImGui::GetColorU32(hov ? c.foreground : c.foreground), title);
    // chevron
    const ImVec2 ctr{pos.x + w - 8.f, pos.y + h * 0.5f};
    const float s = 4.f;
    const ImU32 col = ImGui::GetColorU32(c.muted_foreground);
    if (open) {
        const ImVec2 pts[3] = {{ctr.x - s, ctr.y + s * 0.5f}, {ctr.x, ctr.y - s * 0.5f}, {ctr.x + s, ctr.y + s * 0.5f}};
        dl->AddPolyline(pts, 3, col, 0, 1.5f);
    } else {
        const ImVec2 pts[3] = {{ctr.x - s, ctr.y - s * 0.5f}, {ctr.x, ctr.y + s * 0.5f}, {ctr.x + s, ctr.y - s * 0.5f}};
        dl->AddPolyline(pts, 3, col, 0, 1.5f);
    }
    if (!open) dl->AddLine({pos.x, pos.y + h}, {pos.x + w, pos.y + h}, ImGui::GetColorU32(c.border));
    return open;
}

}  // namespace ui::w
