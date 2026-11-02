#include "core/window.hpp"

#include <dwmapi.h>

#include "core/config.hpp"
#include "imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace core {
namespace {

// Undocumented user32 API used for acrylic blur (Windows 10 1803+ / Windows 11).
struct AccentPolicy { int state; int flags; unsigned gradient; int animation; };
struct WinCompAttrData { int attr; void* data; ULONG size; };
using SetWCA = BOOL(WINAPI*)(HWND, WinCompAttrData*);

constexpr int kAccentBlurBehind = 3;
constexpr int kAccentAcrylic = 4;
constexpr int kWcaAccentPolicy = 19;

}  // namespace

void Window::enable_blur() const {
    auto set_wca = reinterpret_cast<SetWCA>(
        reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute")));
    if (set_wca) {
        for (int state : {kAccentAcrylic, kAccentBlurBehind}) {
            AccentPolicy policy{state, 2, cfg::kTintABGR, 0};
            WinCompAttrData data{kWcaAccentPolicy, &policy, sizeof(policy)};
            if (set_wca(hwnd_, &data)) break;
        }
    }
    // Allow per-pixel alpha from the DX11 swapchain.
    const MARGINS margins{-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd_, &margins);

    // Rounded OS window so the blur is clipped to the same shape as the ImGui window.
    const int d = cfg::kCornerRadius * 2;
    SetWindowRgn(hwnd_, CreateRoundRectRgn(0, 0, cfg::kWidth + 1, cfg::kHeight + 1, d, d), TRUE);
}

bool Window::create(HINSTANCE inst) {
    inst_ = inst;
    WNDCLASSEXW wc{sizeof(wc)};
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = &Window::wnd_proc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = L"CS2LoaderWnd";
    RegisterClassExW(&wc);

    const int x = (GetSystemMetrics(SM_CXSCREEN) - cfg::kWidth) / 2;
    const int y = (GetSystemMetrics(SM_CYSCREEN) - cfg::kHeight) / 2;
    hwnd_ = CreateWindowExW(WS_EX_APPWINDOW, wc.lpszClassName, cfg::kTitle.data(), WS_POPUP | WS_MINIMIZEBOX,
                            x, y, cfg::kWidth, cfg::kHeight, nullptr, nullptr, inst, this);
    if (!hwnd_) return false;

    enable_blur();
    ShowWindow(hwnd_, SW_SHOWDEFAULT);
    UpdateWindow(hwnd_);
    return true;
}

void Window::destroy() {
    if (hwnd_) DestroyWindow(hwnd_);
    UnregisterClassW(L"CS2LoaderWnd", inst_);
    hwnd_ = nullptr;
}

bool Window::pump() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        if (msg.message == WM_QUIT) return false;
    }
    return true;
}

void Window::begin_drag() const {
    ReleaseCapture();
    SendMessageW(hwnd_, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

LRESULT CALLBACK Window::wnd_proc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(l);
        SetWindowLongPtrW(h, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
    }
    if (ImGui_ImplWin32_WndProcHandler(h, msg, w, l)) return true;

    auto* self = reinterpret_cast<Window*>(GetWindowLongPtrW(h, GWLP_USERDATA));
    switch (msg) {
        case WM_SIZE:
            if (self && self->resize_ && w != SIZE_MINIMIZED) self->resize_(LOWORD(l), HIWORD(l));
            return 0;
        case WM_SYSCOMMAND:
            if ((w & 0xfff0) == SC_KEYMENU) return 0;
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(h, msg, w, l);
}

}  // namespace core
