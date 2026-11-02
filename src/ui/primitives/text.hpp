#pragma once
#include <string_view>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

// Primitives: tiny stateless building blocks (shadcn typography).
namespace ui::p {

inline void text_styled(std::string_view s, ImFont* font, float size, const ImVec4& color, bool wrap = false) {
    ImGui::PushFont(font, size);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    if (wrap) ImGui::PushTextWrapPos(0.f);
    ImGui::TextUnformatted(s.data(), s.data() + s.size());
    if (wrap) ImGui::PopTextWrapPos();
    ImGui::PopStyleColor();
    ImGui::PopFont();
}

inline void h1(std::string_view s)    { text_styled(s, fonts::semibold, theme::m.font_xl, theme::c.foreground); }
inline void h2(std::string_view s)    { text_styled(s, fonts::semibold, theme::m.font_lg + 2.f, theme::c.foreground); }
inline void h3(std::string_view s)    { text_styled(s, fonts::semibold, theme::m.font_lg, theme::c.foreground); }
inline void h4(std::string_view s)    { text_styled(s, fonts::semibold, theme::m.font_base + 1.f, theme::c.foreground); }
inline void text(std::string_view s)  { text_styled(s, fonts::regular, theme::m.font_base, theme::c.foreground); }
inline void paragraph(std::string_view s) { text_styled(s, fonts::regular, theme::m.font_base, theme::c.foreground, true); }
inline void label(std::string_view s) { text_styled(s, fonts::medium, theme::m.font_sm, theme::c.foreground); }
inline void muted(std::string_view s) { text_styled(s, fonts::regular, theme::m.font_sm, theme::c.muted_foreground); }
inline void muted_wrap(std::string_view s) { text_styled(s, fonts::regular, theme::m.font_sm, theme::c.muted_foreground, true); }
inline void small(std::string_view s) { text_styled(s, fonts::regular, theme::m.font_xs, theme::c.muted_foreground); }
inline void destructive(std::string_view s) { text_styled(s, fonts::medium, theme::m.font_sm, theme::c.destructive); }

// Width of `s` in the given font/size (for manual layout).
inline float text_width(std::string_view s, ImFont* font, float size) {
    ImGui::PushFont(font, size);
    const float w = ImGui::CalcTextSize(s.data(), s.data() + s.size()).x;
    ImGui::PopFont();
    return w;
}

}  // namespace ui::p
