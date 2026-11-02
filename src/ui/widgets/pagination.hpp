#pragma once
#include <algorithm>
#include <string>

#include "imgui.h"
#include "ui/theme.hpp"
#include "ui/widgets/button.hpp"

namespace ui::w {

// shadcn <Pagination>. page is 0-based. Returns true when the page changed.
inline bool pagination(const char* id, int& page, int pages) {
    if (pages < 1) return false;
    bool changed = false;
    ImGui::PushID(id);
    if (button("<##prev", Variant::Ghost, Size::Md, 32.f) && page > 0) { --page; changed = true; }
    auto num = [&](int i) {
        ImGui::SameLine();
        const std::string l = std::to_string(i + 1) + "##p" + std::to_string(i);
        if (button(l.c_str(), page == i ? Variant::Outline : Variant::Ghost, Size::Md, 32.f) && page != i) { page = i; changed = true; }
    };
    auto dots = [&] {
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("...");
    };
    if (pages <= 7) {
        for (int i = 0; i < pages; ++i) num(i);
    } else {
        num(0);
        if (page > 2) dots();
        for (int i = std::max(1, page - 1); i <= std::min(pages - 2, page + 1); ++i) num(i);
        if (page < pages - 3) dots();
        num(pages - 1);
    }
    ImGui::SameLine();
    if (button(">##next", Variant::Ghost, Size::Md, 32.f) && page < pages - 1) { ++page; changed = true; }
    ImGui::PopID();
    return changed;
}

}  // namespace ui::w
