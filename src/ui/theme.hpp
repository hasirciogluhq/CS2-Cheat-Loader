#pragma once
#include "imgui.h"

// Design tokens — the equivalent of shadcn's index.css (:root / .dark variables).
// Everything is a plain mutable inline variable: change a token at startup (or at runtime),
// then call ui::apply_style() to push it into ImGui. Widgets read tokens every frame.
namespace ui::theme {

constexpr ImVec4 rgb(int r, int g, int b, float a = 1.f) { return {r / 255.f, g / 255.f, b / 255.f, a}; }
constexpr ImVec4 with_alpha(ImVec4 c, float a) { return {c.x, c.y, c.z, a}; }
constexpr ImVec4 mul_alpha(ImVec4 c, float a) { return {c.x, c.y, c.z, c.w * a}; }

struct Colors {
    ImVec4 background, foreground;
    ImVec4 card, card_foreground;
    ImVec4 popover, popover_foreground;
    ImVec4 primary, primary_foreground;
    ImVec4 secondary, secondary_foreground;
    ImVec4 muted, muted_foreground;
    ImVec4 accent, accent_foreground;
    ImVec4 destructive, destructive_foreground;
    ImVec4 border, input, ring;
};

// shadcn "zinc" dark. Background/card are translucent so the acrylic blur shows through.
constexpr Colors zinc_dark() {
    return {
        .background = rgb(9, 9, 11, 0.45f),         .foreground = rgb(250, 250, 250),
        .card = rgb(9, 9, 11, 0.55f),               .card_foreground = rgb(250, 250, 250),
        .popover = rgb(9, 9, 11, 0.96f),            .popover_foreground = rgb(250, 250, 250),
        .primary = rgb(250, 250, 250),              .primary_foreground = rgb(24, 24, 27),
        .secondary = rgb(39, 39, 42, 0.90f),        .secondary_foreground = rgb(250, 250, 250),
        .muted = rgb(39, 39, 42, 0.70f),            .muted_foreground = rgb(161, 161, 170),
        .accent = rgb(39, 39, 42, 0.90f),           .accent_foreground = rgb(250, 250, 250),
        .destructive = rgb(220, 38, 38),            .destructive_foreground = rgb(250, 250, 250),
        .border = rgb(255, 255, 255, 0.10f),        .input = rgb(255, 255, 255, 0.15f),
        .ring = rgb(212, 212, 216),
    };
}

struct Metrics {
    float radius = 8.f;            // --radius (0.5rem); sm/lg derive from it
    float window_radius = 16.f;    // OS window + main ImGui window (see core/config.hpp)
    // spacing scale
    float space_1 = 4.f, space_2 = 8.f, space_3 = 12.f, space_4 = 16.f;
    float window_padding = 12.f;
    float card_padding = 12.f;
    float item_gap = 6.f;          // vertical gap between items
    float inline_gap = 8.f;        // horizontal gap between items
    // control heights
    float control_sm = 26.f, control_md = 32.f, control_lg = 40.f;
    // font sizes
    float font_xs = 12.f, font_sm = 13.f, font_base = 14.f, font_lg = 18.f, font_xl = 22.f;

    float radius_sm() const { return radius - 2.f; }
    float radius_lg() const { return radius + 4.f; }
};

inline Colors c = zinc_dark();
inline Metrics m{};

}  // namespace ui::theme
