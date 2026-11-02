#pragma once
#include <cctype>
#include <cstring>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

namespace ui::w {

namespace detail {
inline bool icontains(const char* hay, const char* needle) {
    if (!*needle) return true;
    for (; *hay; ++hay) {
        const char *h = hay, *n = needle;
        while (*h && *n && std::tolower(static_cast<unsigned char>(*h)) == std::tolower(static_cast<unsigned char>(*n))) { ++h; ++n; }
        if (!*n) return true;
    }
    return false;
}
}  // namespace detail

// shadcn <Combobox>: select with a search box.
inline bool combobox(const char* id, int& index, const char* const* items, int count,
                     const char* placeholder = "Search...", float width = -1.f) {
    using namespace theme;
    static char filter[64];
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
    if (ImGui::BeginCombo(id, index >= 0 ? items[index] : placeholder)) {
        if (ImGui::IsWindowAppearing()) { filter[0] = 0; ImGui::SetKeyboardFocusHere(); }
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputTextWithHint("##filter", placeholder, filter, sizeof filter);
        for (int i = 0; i < count; ++i) {
            if (!detail::icontains(items[i], filter)) continue;
            if (ImGui::Selectable(items[i], i == index)) { changed = i != index; index = i; }
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleVar(5);
    ImGui::PopStyleColor(3);
    ImGui::PopFont();
    return changed;
}

}  // namespace ui::w
