#pragma once
#include <string_view>

namespace cfg {
inline constexpr std::wstring_view kTitle = L"CS2 Loader";
inline constexpr int kWidth = 720;
inline constexpr int kHeight = 500;
inline constexpr int kCornerRadius = 16;  // px, OS window region (== theme::m.window_radius)
inline constexpr unsigned kTintABGR = 0x66100C0A;  // acrylic tint (A,B,G,R)
}  // namespace cfg
