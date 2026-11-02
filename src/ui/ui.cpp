#include "ui/ui.hpp"

#include "core/config.hpp"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ui/fonts.hpp"
#include "ui/primitives/separator.hpp"
#include "ui/primitives/text.hpp"
#include "ui/style.hpp"
#include "ui/widgets/button.hpp"
#include "ui/widgets/card.hpp"

namespace ui {

bool init(core::Window& win, core::Renderer& rend) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    apply_style();
    fonts::load();
    return ImGui_ImplWin32_Init(win.handle()) && ImGui_ImplDX11_Init(rend.device(), rend.context());
}

void shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void frame(core::Window& win) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({static_cast<float>(cfg::kWidth), static_cast<float>(cfg::kHeight)});
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar;
    ImGui::Begin("##loader", nullptr, flags);

    // Title bar (drag area) + close button.
    p::h1("CS2 Loader");
    ImGui::SameLine(ImGui::GetWindowWidth() - 20.f - 36.f);
    if (w::button("X", w::Variant::Secondary, {36.f, 32.f})) win.request_close();
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::GetMousePos().y < 52.f &&
        !ImGui::IsAnyItemHovered())
        win.begin_drag();
    p::spacer(4.f);

    if (w::card_begin("##main", "Status", "Empty loader template.", {0.f, 0.f})) {
        p::label("Ready");
        p::small("Loader logic goes here.");
        p::spacer(10.f);
        w::button("Load");
        ImGui::SameLine();
        w::button("Settings", w::Variant::Secondary);
    }
    w::card_end();

    ImGui::End();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace ui
