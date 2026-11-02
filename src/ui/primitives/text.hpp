#pragma once
#include <string>
#include <string_view>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

// Primitives: tiny, stateless building blocks (shadcn typography).
namespace ui::p {

inline void text_styled(std::string_view s, ImFont* font, float size, const ImVec4& color) {
    ImGui::PushFont(font, size);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    const std::string str(s);   // ensure null-terminated
    ImGui::TextUnformatted(str.c_str());
    ImGui::PopStyleColor();
    ImGui::PopFont();
}

inline void h1(std::string_view s)    { text_styled(s, fonts::semibold, 24.f, theme::foreground); }
inline void h3(std::string_view s)    { text_styled(s, fonts::semibold, 18.f, theme::foreground); }
inline void text(std::string_view s)  { text_styled(s, fonts::regular, 14.f, theme::foreground); }
inline void label(std::string_view s) { text_styled(s, fonts::medium, 14.f, theme::foreground); }
inline void muted(std::string_view s) { text_styled(s, fonts::regular, 14.f, theme::muted_foreground); }
inline void small(std::string_view s) { text_styled(s, fonts::regular, 12.f, theme::muted_foreground); }

}  // namespace ui::p
