#pragma once
#include <windows.h>
#include <functional>

namespace core {

class Window {
public:
    using ResizeFn = std::function<void(UINT, UINT)>;

    bool create(HINSTANCE inst);
    void destroy();
    bool pump();                       // returns false after WM_QUIT
    void begin_drag() const;           // start moving borderless window
    void request_close() const { PostMessageW(hwnd_, WM_CLOSE, 0, 0); }

    HWND handle() const { return hwnd_; }
    void on_resize(ResizeFn fn) { resize_ = std::move(fn); }

private:
    static LRESULT CALLBACK wnd_proc(HWND, UINT, WPARAM, LPARAM);
    void enable_blur() const;

    HWND hwnd_{};
    HINSTANCE inst_{};
    ResizeFn resize_;
};

}  // namespace core
