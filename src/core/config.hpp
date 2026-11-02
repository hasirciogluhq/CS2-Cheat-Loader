#pragma once
#include <string_view>

namespace cfg {
inline constexpr std::wstring_view kTitle = L"CS2 Loader";
inline constexpr int kWidth = 520;
inline constexpr int kHeight = 340;
inline constexpr int kCornerRadius = 16;  // px, window + imgui rounding
inline constexpr unsigned kTintABGR = 0x66100C0A;  // acrylic tint (A,B,G,R)
}  // namespace cfg
