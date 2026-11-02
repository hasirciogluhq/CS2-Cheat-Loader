#include "ui/ui.hpp"

#include "core/config.hpp"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ui/fonts.hpp"
#include "ui/style.hpp"
#include "ui/theme.hpp"
#include "ui/widgets/widgets.hpp"

namespace ui {

bool init(core::Window& win, core::Renderer& rend) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    theme::m.window_radius = static_cast<float>(cfg::kCornerRadius);
    apply_style();
    fonts::load();
    return ImGui_ImplWin32_Init(win.handle()) && ImGui_ImplDX11_Init(rend.device(), rend.context());
}

void shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

namespace {

void header(core::Window& win) {
    p::h3("CS2 Loader");
    p::same_line();
    w::badge("v0.1.0", w::BadgeVariant::Secondary);
    ImGui::SameLine(ImGui::GetWindowWidth() - theme::m.window_padding - 28.f);
    if (w::button("X##close", w::Variant::Ghost, w::Size::Sm, 28.f)) win.request_close();
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::GetMousePos().y < 40.f && !ImGui::IsAnyItemHovered())
        win.begin_drag();
}

void tab_loader() {
    const float cw = p::column_width(2);
    if (w::card_begin("##status", "Status", "Loader is ready.", {cw, 0.f})) {
        w::badge("Undetected", w::BadgeVariant::Default);
        p::spacer(2.f);
        p::muted("Preparing...");
        static float prog = 0.35f;
        w::progress(prog);
        p::spacer(2.f);
        if (w::button("Load", w::Variant::Default, w::Size::Md, -1.f)) prog = prog >= 1.f ? 0.f : prog + 0.25f;
    }
    w::card_end();
    p::same_line();
    if (w::card_begin("##account", "Account", "Signed in.", {cw, 0.f})) {
        w::avatar("HS");
        p::same_line();
        ImGui::BeginGroup();
        p::label("Player");
        p::small("Subscription: Lifetime");
        ImGui::EndGroup();
        p::spacer(2.f);
        w::button("Logout", w::Variant::Outline, w::Size::Md, -1.f);
    }
    w::card_end();
    w::alert("##note", "Heads up", "This is an empty template, plug your logic into ui/ui.cpp.");
}

void tab_settings() {
    static char name[64] = "";
    static int game = 0;
    static float volume = 60.f;
    static bool a = true, b = false, c = true;
    static const char* const items[] = {"Counter-Strike 2", "Other"};

    if (w::card_begin("##settings", "Settings", "Tweak the loader.")) {
        w::field_label("Username");
        w::input("##name", name, "Enter username");
        p::spacer(2.f);
        w::field_label("Target");
        w::select("##target", game, items, 2);
        p::spacer(2.f);
        w::field_label("Volume");
        w::slider("##vol", volume, 0.f, 100.f);
        p::spacer(2.f);
        w::toggle("Auto update", a);
        w::toggle("Close on inject", b);
        w::checkbox("Remember me", c);
        w::tooltip("Stores your username locally");
    }
    w::card_end();
}

void tab_components() {
    if (w::card_begin("##buttons", "Buttons")) {
        w::button("Default");
        p::same_line();
        w::button("Secondary", w::Variant::Secondary);
        p::same_line();
        w::button("Outline", w::Variant::Outline);
        p::same_line();
        w::button("Ghost", w::Variant::Ghost);
        p::same_line();
        w::button("Delete", w::Variant::Destructive);
        p::spacer(2.f);
        w::button("Small", w::Variant::Secondary, w::Size::Sm);
        p::same_line();
        w::button("Large", w::Variant::Secondary, w::Size::Lg);
    }
    w::card_end();
    if (w::card_begin("##misc", "Badges & more")) {
        w::badge("Default");
        p::same_line(6.f);
        w::badge("Secondary", w::BadgeVariant::Secondary);
        p::same_line(6.f);
        w::badge("Outline", w::BadgeVariant::Outline);
        p::same_line(6.f);
        w::badge("Destructive", w::BadgeVariant::Destructive);
        p::same_line(16.f);
        w::kbd("Ctrl");
        p::same_line(4.f);
        w::kbd("K");
        p::spacer(2.f);
        p::spinner();
        p::same_line();
        p::muted("Loading...");
    }
    w::card_end();
    w::alert("##err", "Error", "Something went wrong.", true);
}

void tab_more() {
    static int radio = 0, seg = 0, page = 0, num = 5, combo = -1;
    static bool pressed = false;
    static char notes[256] = "";
    static const char* const games[] = {"Counter-Strike 2", "Dota 2", "Valorant", "Apex Legends"};

    w::breadcrumb("bc", {"Home", "Loader", "More"});
    p::spacer(2.f);
    if (w::card_begin("##forms", "Forms")) {
        w::textarea("##notes", notes, 60.f);
        p::spacer(2.f);
        w::radio_group("rg", radio, {"Release", "Beta", "Nightly"});
        p::spacer(2.f);
        w::toggle_group("tg", {"Day", "Week", "Month"}, seg);
        p::same_line();
        w::toggle_button("Bold", pressed);
        p::spacer(2.f);
        w::combobox("##combo", combo, games, 4, "Select game...");
        p::spacer(2.f);
        w::input_number("num", num, 0, 20);
    }
    w::card_end();
    if (w::card_begin("##overlays", "Overlays")) {
        if (w::button("Dialog", w::Variant::Outline)) w::open_popup("dlg");
        p::same_line();
        if (w::button("Menu", w::Variant::Outline)) w::open_popup("menu");
        if (w::menu_begin("menu")) {
            w::menu_label("My Account");
            w::menu_item("Profile", "Ctrl+P");
            w::menu_item("Settings", "Ctrl+S");
            w::menu_separator();
            if (w::menu_item("Log out", nullptr, true)) w::toast("Logged out", "See you soon.");
            w::menu_end();
        }
        p::same_line();
        if (w::button("Popover", w::Variant::Outline)) w::open_popup("pop");
        if (w::popover_begin("pop")) {
            p::h4("Dimensions");
            p::muted("Set the dimensions for the layer.");
            w::menu_end();
        }
        p::same_line();
        if (w::button("Toast", w::Variant::Outline)) w::toast("Saved", "Your changes were saved.");
        p::same_line();
        if (w::button("Error toast", w::Variant::Outline)) w::toast("Failed", "Something went wrong.", 3.f, true);

        if (w::dialog_begin("dlg", "Are you sure?", "This action cannot be undone.")) {
            if (w::button("Cancel", w::Variant::Outline)) w::close_popup();
            p::same_line();
            if (w::button("Continue")) w::close_popup();
            w::dialog_end();
        }
    }
    w::card_end();
    if (w::card_begin("##data", "Data")) {
        if (w::accordion("What is this?", true)) p::muted_wrap("A shadcn-style widget kit built on ImGui.");
        if (w::table_begin("tbl", {"Name", "Status", "Ping"})) {
            w::table_row(); p::text("Frankfurt"); w::table_cell(); w::badge("Online"); w::table_cell(); p::muted("12 ms");
            w::table_row(); p::text("Istanbul"); w::table_cell(); w::badge("Busy", w::BadgeVariant::Secondary); w::table_cell(); p::muted("34 ms");
            w::table_end();
        }
        p::spacer(2.f);
        w::pagination("pg", page, 10);
        p::spacer(2.f);
        w::skeleton(0.f, 12.f);
        w::skeleton(160.f, 12.f);
        p::separator_label("OR");
    }
    w::card_end();
}

}  // namespace

void frame(core::Window& win) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({static_cast<float>(cfg::kWidth), static_cast<float>(cfg::kHeight)});
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar;
    ImGui::Begin("##loader", nullptr, flags);

    header(win);
    static int tab = 0;
    w::tabs("main", {"Loader", "Settings", "Components", "More"}, tab);
    p::spacer(2.f);
    if (w::scroll_area_begin("##body")) {
        switch (tab) {
            case 0: tab_loader(); break;
            case 1: tab_settings(); break;
            case 2: tab_components(); break;
            case 3: tab_more(); break;
        }
    }
    w::scroll_area_end();

    ImGui::End();
    w::toast_render();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace ui
