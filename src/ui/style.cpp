#include "ui/style.hpp"

#include "core/config.hpp"
#include "imgui.h"
#include "ui/theme.hpp"

namespace ui {

void apply_style() {
    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = static_cast<float>(cfg::kCornerRadius);   // matches OS window region
    s.ChildRounding = theme::radius + 4.f;
    s.FrameRounding = theme::radius;
    s.PopupRounding = theme::radius;
    s.GrabRounding = theme::radius;
    s.WindowBorderSize = 1.f;
    s.FrameBorderSize = 0.f;
    s.WindowPadding = {20.f, 16.f};
    s.FramePadding = {14.f, 8.f};
    s.ItemSpacing = {10.f, 10.f};

    using namespace theme;
    auto* c = s.Colors;
    c[ImGuiCol_Text] = foreground;
    c[ImGuiCol_TextDisabled] = muted_foreground;
    c[ImGuiCol_WindowBg] = background;
    c[ImGuiCol_ChildBg] = card;
    c[ImGuiCol_PopupBg] = rgba(9, 9, 11, 0.95f);
    c[ImGuiCol_Border] = border;
    c[ImGuiCol_FrameBg] = muted;
    c[ImGuiCol_FrameBgHovered] = secondary_hover;
    c[ImGuiCol_FrameBgActive] = secondary_hover;
    c[ImGuiCol_Button] = secondary;
    c[ImGuiCol_ButtonHovered] = secondary_hover;
    c[ImGuiCol_ButtonActive] = secondary_hover;
    c[ImGuiCol_Separator] = border;
}

}  // namespace ui
