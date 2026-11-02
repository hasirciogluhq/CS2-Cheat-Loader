#pragma once
#include <array>
#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/primitives/layout.hpp"
#include "ui/theme.hpp"

namespace ui::w {

namespace detail {
inline void label_text(ImVec2 pos, float h, float x, const char* label) {
    ImGui::PushFont(fonts::regular, theme::m.font_base);
    const char* end = p::label_end(label);
    const float th = ImGui::GetFontSize();
    ImGui::GetWindowDrawList()->AddText(fonts::regular, theme::m.font_base, {pos.x + x, pos.y + (h - th) * 0.5f},
                                        ImGui::GetColorU32(theme::c.foreground), label, end);
    ImGui::PopFont();
}
inline float label_width(const char* label) {
    ImGui::PushFont(fonts::regular, theme::m.font_base);
    const float w = ImGui::CalcTextSize(label, p::label_end(label), true).x;
    ImGui::PopFont();
    return w;
}
}  // namespace detail

// shadcn <Checkbox>. Returns true when toggled.
inline bool checkbox(const char* label, bool& v) {
    using namespace theme;
    constexpr float box = 16.f;
    const float gap = m.space_2;
    const float lw = detail::label_width(label);
    const float h = m.control_sm - 4.f;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(label, {box + (lw > 0.f ? gap + lw : 0.f), h});
    const bool clicked = ImGui::IsItemClicked();
    if (clicked) v = !v;

    auto* dl = ImGui::GetWindowDrawList();
    const ImVec2 a{pos.x, pos.y + (h - box) * 0.5f}, b{a.x + box, a.y + box};
    const bool hov = ImGui::IsItemHovered();
    dl->AddRectFilled(a, b, ImGui::GetColorU32(v ? c.primary : with_alpha(c.input, hov ? 0.16f : 0.08f)), m.radius_sm() - 2.f);
    dl->AddRect(a, b, ImGui::GetColorU32(v ? c.primary : c.input), m.radius_sm() - 2.f);
    if (v) {
        const ImU32 col = ImGui::GetColorU32(c.primary_foreground);
        dl->AddPolyline(std::array<ImVec2, 3>{ImVec2{a.x + 3.5f, a.y + 8.5f}, ImVec2{a.x + 6.8f, a.y + 11.5f},
                                              ImVec2{a.x + 12.f, a.y + 4.8f}}.data(), 3, col, 0, 2.f);
    }
    detail::label_text(pos, h, box + gap, label);
    return clicked;
}

// shadcn <Switch>. Returns true when toggled.
inline bool toggle(const char* label, bool& v) {
    using namespace theme;
    constexpr float tw = 32.f, th = 18.f;
    const float gap = m.space_2;
    const float lw = detail::label_width(label);
    const float h = m.control_sm - 2.f;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(label, {tw + (lw > 0.f ? gap + lw : 0.f), h});
    const bool clicked = ImGui::IsItemClicked();
    if (clicked) v = !v;

    auto* dl = ImGui::GetWindowDrawList();
    const ImVec2 a{pos.x, pos.y + (h - th) * 0.5f}, b{a.x + tw, a.y + th};
    dl->AddRectFilled(a, b, ImGui::GetColorU32(v ? c.primary : with_alpha(c.input, 0.25f)), 999.f);
    const float r = th * 0.5f - 2.f;
    const float cx = v ? b.x - th * 0.5f : a.x + th * 0.5f;
    dl->AddCircleFilled({cx, a.y + th * 0.5f}, r, ImGui::GetColorU32(v ? c.primary_foreground : c.foreground), 24);
    detail::label_text(pos, h, tw + gap, label);
    return clicked;
}

}  // namespace ui::w
