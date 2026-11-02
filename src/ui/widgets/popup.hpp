#pragma once
#include <string_view>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/primitives/layout.hpp"
#include "ui/primitives/text.hpp"
#include "ui/theme.hpp"

// Overlays: dropdown menu, context menu, popover, dialog.
// Convention (like ImGui): call the matching *_end() ONLY when *_begin() returned true.
namespace ui::w {

namespace detail {
inline void push_popup_style(float pad) {
    using namespace theme;
    ImGui::PushStyleColor(ImGuiCol_PopupBg, c.popover);
    ImGui::PushStyleColor(ImGuiCol_Border, c.border);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, m.radius_lg() - 2.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {pad, pad});
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, m.radius_sm());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {m.inline_gap, 2.f});
}
inline void pop_popup_style() { ImGui::PopStyleVar(4); ImGui::PopStyleColor(2); }

inline bool begin_anchored(const char* id, float min_w, float pad) {
    // Anchor below the previous item (the trigger).
    const ImVec2 a = ImGui::GetItemRectMin(), b = ImGui::GetItemRectMax();
    ImGui::SetNextWindowPos({a.x, b.y + 4.f}, ImGuiCond_Appearing);
    ImGui::SetNextWindowSizeConstraints({min_w, 0.f}, {FLT_MAX, FLT_MAX});
    push_popup_style(pad);
    ImGui::PushFont(fonts::regular, theme::m.font_base);
    if (ImGui::BeginPopup(id, ImGuiWindowFlags_NoSavedSettings)) return true;
    ImGui::PopFont();
    pop_popup_style();
    return false;
}
}  // namespace detail

inline void open_popup(const char* id) { ImGui::OpenPopup(id); }
inline void close_popup() { ImGui::CloseCurrentPopup(); }

// ---- Dropdown menu: if (w::button("Menu")) w::open_popup("m"); if (w::menu_begin("m")) {...; w::menu_end();}
inline bool menu_begin(const char* id, float min_width = 180.f) { return detail::begin_anchored(id, min_width, theme::m.space_1); }
inline void menu_end() { ImGui::EndPopup(); ImGui::PopFont(); detail::pop_popup_style(); }

// Right-click context menu for the previous item.
inline bool context_menu_begin(const char* id, float min_width = 180.f) {
    detail::push_popup_style(theme::m.space_1);
    ImGui::SetNextWindowSizeConstraints({min_width, 0.f}, {FLT_MAX, FLT_MAX});
    ImGui::PushFont(fonts::regular, theme::m.font_base);
    if (ImGui::BeginPopupContextItem(id)) return true;
    ImGui::PopFont();
    detail::pop_popup_style();
    return false;
}

inline bool menu_item(const char* label, const char* shortcut = nullptr, bool destructive = false) {
    using namespace theme;
    ImGui::PushStyleColor(ImGuiCol_Text, destructive ? c.destructive : c.popover_foreground);
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, {0.f, 0.5f});
    const bool clicked = ImGui::Selectable(label, false, 0, {0.f, m.control_sm + 2.f});
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    if (shortcut) {
        const ImVec2 a = ImGui::GetItemRectMin(), b = ImGui::GetItemRectMax();
        const float tw = ImGui::CalcTextSize(shortcut).x;
        ImGui::GetWindowDrawList()->AddText({b.x - tw - m.space_2, a.y + (b.y - a.y - ImGui::GetFontSize()) * 0.5f},
                                            ImGui::GetColorU32(c.muted_foreground), shortcut);
    }
    return clicked;
}
inline void menu_label(const char* text) {
    ImGui::Dummy({0.f, 2.f});
    ImGui::Indent(theme::m.space_2);
    p::label(text);
    ImGui::Unindent(theme::m.space_2);
}
inline void menu_separator() { ImGui::Dummy({0.f, 1.f}); ImGui::Separator(); ImGui::Dummy({0.f, 1.f}); }

// ---- Popover: free-form content below the trigger.
inline bool popover_begin(const char* id, float width = 260.f) {
    return detail::begin_anchored(id, width, theme::m.space_3);
}
inline void popover_end() { menu_end(); }

// ---- Dialog (modal, centered, dimmed backdrop).
inline bool dialog_begin(const char* id, std::string_view title, std::string_view description = {}, float width = 360.f) {
    using namespace theme;
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, {0.5f, 0.5f});
    ImGui::SetNextWindowSizeConstraints({width, 0.f}, {width, FLT_MAX});
    ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4{0.f, 0.f, 0.f, 0.55f});
    detail::push_popup_style(m.space_4);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {m.inline_gap, m.item_gap + 2.f});
    constexpr ImGuiWindowFlags fl = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar;
    if (ImGui::BeginPopupModal(id, nullptr, fl)) {
        p::h3(title);
        if (!description.empty()) p::muted_wrap(description);
        p::spacer(2.f);
        return true;
    }
    ImGui::PopStyleVar();
    detail::pop_popup_style();
    ImGui::PopStyleColor();
    return false;
}
inline void dialog_end() {
    ImGui::EndPopup();
    ImGui::PopStyleVar();
    detail::pop_popup_style();
    ImGui::PopStyleColor();
}

}  // namespace ui::w
