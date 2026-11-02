#pragma once
#include <string_view>

#include "imgui.h"
#include "ui/primitives/layout.hpp"
#include "ui/primitives/separator.hpp"
#include "ui/primitives/text.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Card> (+ header). Always pair with card_end().
//   if (w::card_begin("id", "Title", "Description")) { ... }  w::card_end();
// size.x: 0 = full width, size.y: 0 = fit content (auto height) unless auto_height=false.
inline bool card_begin(const char* id, std::string_view title = {}, std::string_view description = {},
                       ImVec2 size = {0.f, 0.f}, bool auto_height = true) {
    using namespace theme;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, c.card);
    ImGui::PushStyleColor(ImGuiCol_Border, c.border);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, m.radius_lg());
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {m.card_padding, m.card_padding});
    ImGuiChildFlags cf = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;
    if (auto_height && size.y == 0.f) cf |= ImGuiChildFlags_AutoResizeY;
    const bool open = ImGui::BeginChild(id, size, cf, ImGuiWindowFlags_NoScrollbar);
    if (open) {
        if (!title.empty()) p::h4(title);
        if (!description.empty()) p::muted_wrap(description);
        if (!title.empty() || !description.empty()) p::spacer(2.f);
    }
    return open;
}

inline void card_end() {
    ImGui::EndChild();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

}  // namespace ui::w
