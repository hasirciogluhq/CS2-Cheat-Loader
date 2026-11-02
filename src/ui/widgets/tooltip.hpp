#pragma once
#include "imgui.h"
#include "ui/primitives/text.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Tooltip>: call right after the item it describes.
inline void tooltip(const char* text) {
    using namespace theme;
    if (!ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay)) return;
    ImGui::PushStyleColor(ImGuiCol_PopupBg, c.popover);
    ImGui::PushStyleColor(ImGuiCol_Border, c.border);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {m.space_2 + 2.f, m.space_1 + 2.f});
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, m.radius_sm());
    if (ImGui::BeginTooltip()) { p::text_styled(text, fonts::regular, m.font_xs + 1.f, c.popover_foreground); ImGui::EndTooltip(); }
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

}  // namespace ui::w
