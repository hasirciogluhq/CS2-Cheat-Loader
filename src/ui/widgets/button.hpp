#pragma once
#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/primitives/layout.hpp"
#include "ui/theme.hpp"

namespace ui::w {

enum class Variant { Default, Secondary, Outline, Ghost, Destructive };
enum class Size { Sm, Md, Lg };

// shadcn <Button>. width: 0 = fit text, <0 = fill available, >0 = fixed. Returns true on click.
inline bool button(const char* label, Variant v = Variant::Default, Size sz = Size::Md, float width = 0.f) {
    using namespace theme;
    const float h = sz == Size::Sm ? m.control_sm : sz == Size::Lg ? m.control_lg : m.control_md;
    const float fs = sz == Size::Sm ? m.font_xs + 1.f : sz == Size::Lg ? m.font_base + 1.f : m.font_base;
    const float padx = sz == Size::Sm ? m.space_3 : sz == Size::Lg ? m.space_4 + 8.f : m.space_4;

    ImVec4 bg, hov, act, fg, border{0, 0, 0, 0};
    switch (v) {
        case Variant::Default:
            bg = c.primary; hov = mul_alpha(c.primary, 0.90f); act = mul_alpha(c.primary, 0.80f);
            fg = c.primary_foreground; break;
        case Variant::Secondary:
            bg = c.secondary; hov = mul_alpha(c.secondary, 0.80f); act = mul_alpha(c.secondary, 0.65f);
            fg = c.secondary_foreground; break;
        case Variant::Outline:
            bg = {0, 0, 0, 0}; hov = c.accent; act = c.accent;
            fg = c.foreground; border = c.input; break;
        case Variant::Ghost:
            bg = {0, 0, 0, 0}; hov = c.accent; act = c.accent;
            fg = c.foreground; break;
        case Variant::Destructive:
            bg = c.destructive; hov = mul_alpha(c.destructive, 0.90f); act = mul_alpha(c.destructive, 0.80f);
            fg = c.destructive_foreground; break;
    }

    ImGui::PushFont(fonts::medium, fs);
    const char* end = p::label_end(label);
    float w = width;
    if (w == 0.f) w = ImGui::CalcTextSize(label, end, true).x + padx * 2.f;
    else if (w < 0.f) w = ImGui::GetContentRegionAvail().x;

    ImGui::PushStyleColor(ImGuiCol_Button, bg);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hov);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, act);
    ImGui::PushStyleColor(ImGuiCol_Text, fg);
    ImGui::PushStyleColor(ImGuiCol_Border, border);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, sz == Size::Sm ? m.radius_sm() : m.radius);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, border.w > 0.f ? 1.f : 0.f);
    const bool clicked = ImGui::Button(label, {w, h});
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(5);
    ImGui::PopFont();
    return clicked;
}

}  // namespace ui::w
