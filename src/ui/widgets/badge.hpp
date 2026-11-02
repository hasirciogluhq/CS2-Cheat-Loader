#pragma once
#include <string_view>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

namespace ui::w {

enum class BadgeVariant { Default, Secondary, Outline, Destructive };

namespace detail {
// Rounded text chip, the shared base of badge and kbd.
inline void chip(std::string_view s, ImFont* font, float fs, ImVec4 bg, ImVec4 fg, ImVec4 border, float rounding,
                 ImVec2 pad) {
    ImGui::PushFont(font, fs);
    const ImVec2 ts = ImGui::CalcTextSize(s.data(), s.data() + s.size());
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImVec2 size{ts.x + pad.x * 2.f, ts.y + pad.y * 2.f};
    ImGui::Dummy(size);
    auto* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(pos, {pos.x + size.x, pos.y + size.y}, ImGui::GetColorU32(bg), rounding);
    if (border.w > 0.f)
        dl->AddRect(pos, {pos.x + size.x, pos.y + size.y}, ImGui::GetColorU32(border), rounding);
    dl->AddText(font, fs, {pos.x + pad.x, pos.y + pad.y}, ImGui::GetColorU32(fg), s.data(), s.data() + s.size());
    ImGui::PopFont();
}
}  // namespace detail

// shadcn <Badge>.
inline void badge(std::string_view s, BadgeVariant v = BadgeVariant::Default) {
    using namespace theme;
    ImVec4 bg{0, 0, 0, 0}, fg = c.foreground, border{0, 0, 0, 0};
    switch (v) {
        case BadgeVariant::Default: bg = c.primary; fg = c.primary_foreground; break;
        case BadgeVariant::Secondary: bg = c.secondary; fg = c.secondary_foreground; break;
        case BadgeVariant::Outline: border = c.input; break;
        case BadgeVariant::Destructive: bg = c.destructive; fg = c.destructive_foreground; break;
    }
    detail::chip(s, fonts::semibold, m.font_xs, bg, fg, border, 999.f, {8.f, 2.f});
}

// Keyboard key hint, e.g. w::kbd("Ctrl").
inline void kbd(std::string_view s) {
    using namespace theme;
    detail::chip(s, fonts::medium, m.font_xs, c.muted, c.muted_foreground, {0, 0, 0, 0}, m.radius_sm(), {6.f, 2.f});
}

}  // namespace ui::w
