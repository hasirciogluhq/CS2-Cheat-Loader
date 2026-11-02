#include "ui/style.hpp"

#include "imgui.h"
#include "ui/theme.hpp"

namespace ui {

void apply_style() {
    using namespace theme;
    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = m.window_radius;
    s.ChildRounding = m.radius_lg();
    s.FrameRounding = m.radius;
    s.PopupRounding = m.radius;
    s.GrabRounding = m.radius;
    s.ScrollbarRounding = m.radius;
    s.WindowBorderSize = 1.f;
    s.ChildBorderSize = 1.f;
    s.PopupBorderSize = 1.f;
    s.FrameBorderSize = 0.f;
    s.WindowPadding = {m.window_padding, m.window_padding};
    s.FramePadding = {m.space_3, 6.f};
    s.ItemSpacing = {m.inline_gap, m.item_gap};
    s.ItemInnerSpacing = {m.space_2, m.space_1};
    s.ScrollbarSize = 8.f;
    s.WindowTitleAlign = {0.f, 0.5f};

    auto* k = s.Colors;
    k[ImGuiCol_Text] = c.foreground;
    k[ImGuiCol_TextDisabled] = c.muted_foreground;
    k[ImGuiCol_WindowBg] = c.background;
    k[ImGuiCol_ChildBg] = c.card;
    k[ImGuiCol_PopupBg] = c.popover;
    k[ImGuiCol_Border] = c.border;
    k[ImGuiCol_FrameBg] = with_alpha(c.input, 0.08f);
    k[ImGuiCol_FrameBgHovered] = with_alpha(c.input, 0.14f);
    k[ImGuiCol_FrameBgActive] = with_alpha(c.input, 0.14f);
    k[ImGuiCol_Button] = c.secondary;
    k[ImGuiCol_ButtonHovered] = c.accent;
    k[ImGuiCol_ButtonActive] = c.accent;
    k[ImGuiCol_Header] = c.accent;
    k[ImGuiCol_HeaderHovered] = c.accent;
    k[ImGuiCol_HeaderActive] = c.accent;
    k[ImGuiCol_Separator] = c.border;
    k[ImGuiCol_ScrollbarBg] = {0, 0, 0, 0};
    k[ImGuiCol_ScrollbarGrab] = c.muted;
    k[ImGuiCol_TextSelectedBg] = with_alpha(c.primary, 0.25f);
    k[ImGuiCol_NavCursor] = {0, 0, 0, 0};
}

}  // namespace ui
