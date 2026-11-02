#pragma once
#include <string_view>

#include "imgui.h"
#include "ui/primitives/text.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Alert>. Full width, fits content.
inline void alert(const char* id, std::string_view title, std::string_view description = {}, bool destructive = false) {
    using namespace theme;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, destructive ? with_alpha(c.destructive, 0.10f) : c.card);
    ImGui::PushStyleColor(ImGuiCol_Border, destructive ? with_alpha(c.destructive, 0.5f) : c.border);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, m.radius_lg());
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {m.space_3, m.space_2 + 2.f});
    if (ImGui::BeginChild(id, {0.f, 0.f}, ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding,
                          ImGuiWindowFlags_NoScrollbar)) {
        if (destructive) p::destructive(title); else p::label(title);
        if (!description.empty()) p::muted_wrap(description);
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

}  // namespace ui::w
