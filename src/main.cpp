#include <windows.h>

#include <cstdio>
#include <print>

#include "core/renderer.hpp"
#include "core/window.hpp"
#include "ui/ui.hpp"

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR, int) {
#ifdef LOADER_DEBUG
    if (AllocConsole()) {
        std::freopen("CONOUT$", "w", stdout);
        std::freopen("CONOUT$", "w", stderr);
    }
    std::println("[CS2Loader] debug build");
#endif

    core::Window window;
    core::Renderer renderer;

    if (!window.create(inst)) return 1;
    if (!renderer.init(window.handle())) {
        MessageBoxW(nullptr, L"Failed to create D3D11 device", L"CS2 Loader", MB_ICONERROR);
        return 2;
    }
    window.on_resize([&](UINT w, UINT h) { renderer.resize(w, h); });
    if (!ui::init(window, renderer)) return 3;

    while (window.pump()) {
        renderer.begin_frame();
        ui::frame(window);
        renderer.end_frame();
    }

    ui::shutdown();
    renderer.shutdown();
    window.destroy();
    return 0;
}
