#pragma once
#include <string>
#include <string_view>

#include "imgui.h"
#include "ui/primitives/separator.hpp"
#include "ui/primitives/text.hpp"
#include "ui/theme.hpp"

// Widgets: composed from primitives + ImGui.
namespace ui::w {

// shadcn <Card>: rounded bordered container with title + description.
//   if (ui::w::card_begin("id", "Title", "Description")) { ...; } ui::w::card_end();
inline bool card_begin(const char* id, std::string_view title, std::string_view description = {},
                       ImVec2 size = {0.f, 0.f}) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, theme::card);
    ImGui::PushStyleColor(ImGuiCol_Border, theme::border);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, theme::radius + 4.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {20.f, 18.f});
    const bool open = ImGui::BeginChild(id, size, ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding,
                                        ImGuiWindowFlags_NoScrollbar);
    if (open) {
        p::h3(title);
        if (!description.empty()) p::muted(description);
        p::spacer(6.f);
    }
    return open;
}

inline void card_end() {
    ImGui::EndChild();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

}  // namespace ui::w
