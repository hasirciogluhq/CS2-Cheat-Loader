#pragma once
#include "imgui.h"
#include "ui/theme.hpp"

namespace ui::p {

inline void separator() {
    ImGui::PushStyleColor(ImGuiCol_Separator, theme::border);
    ImGui::Separator();
    ImGui::PopStyleColor();
}

inline void spacer(float h = 8.f) { ImGui::Dummy({0.f, h}); }

}  // namespace ui::p
