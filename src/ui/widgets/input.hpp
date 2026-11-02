#pragma once
#include <cstddef>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/primitives/text.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Input>. `buf` is a char array (e.g. char name[64]). width: <0 fill (default), 0 default, >0 fixed.
inline bool input(const char* id, char* buf, size_t size, const char* placeholder = "", float width = -1.f,
                  ImGuiInputTextFlags flags = 0) {
    using namespace theme;
    ImGui::PushFont(fonts::regular, m.font_base);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, with_alpha(c.input, 0.08f));
    ImGui::PushStyleColor(ImGuiCol_Border, c.input);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, m.radius);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {m.space_3 - 2.f, (m.control_md - ImGui::GetFontSize()) * 0.5f});
    if (width != 0.f) ImGui::SetNextItemWidth(width < 0.f ? -FLT_MIN : width);
    const bool changed = ImGui::InputTextWithHint(id, placeholder, buf, size, flags);
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
    ImGui::PopFont();
    return changed;
}

template <size_t N>
bool input(const char* id, char (&buf)[N], const char* placeholder = "", float width = -1.f) {
    return input(id, buf, N, placeholder, width);
}

// Label above a control (shadcn <Label> / <Field>). Use before input/select/slider.
inline void field_label(const char* text) { p::label(text); }

}  // namespace ui::w
