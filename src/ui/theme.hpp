#pragma once
#include "imgui.h"

// shadcn/ui "zinc" dark palette (https://ui.shadcn.com/themes).
namespace ui::theme {

constexpr ImVec4 rgba(int r, int g, int b, float a = 1.f) { return {r / 255.f, g / 255.f, b / 255.f, a}; }

inline constexpr ImVec4 background       = rgba(9, 9, 11, 0.45f);      // zinc-950, translucent so blur shows
inline constexpr ImVec4 card             = rgba(9, 9, 11, 0.60f);
inline constexpr ImVec4 border           = rgba(39, 39, 42);           // zinc-800
inline constexpr ImVec4 foreground       = rgba(250, 250, 250);        // zinc-50
inline constexpr ImVec4 muted            = rgba(39, 39, 42);
inline constexpr ImVec4 muted_foreground = rgba(161, 161, 170);        // zinc-400
inline constexpr ImVec4 primary          = rgba(250, 250, 250);
inline constexpr ImVec4 primary_hover    = rgba(250, 250, 250, 0.90f);
inline constexpr ImVec4 primary_active   = rgba(250, 250, 250, 0.80f);
inline constexpr ImVec4 primary_fg       = rgba(24, 24, 27);           // zinc-900
inline constexpr ImVec4 secondary        = rgba(39, 39, 42);
inline constexpr ImVec4 secondary_hover  = rgba(63, 63, 70);           // zinc-700
inline constexpr ImVec4 destructive      = rgba(127, 29, 29);

inline constexpr float radius = 8.f;   // 0.5rem

}  // namespace ui::theme
