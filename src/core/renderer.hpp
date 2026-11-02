#pragma once
#include <d3d11.h>
#include <dxgi.h>

namespace core {

class Renderer {
public:
    bool init(HWND hwnd);
    void shutdown();
    void resize(UINT w, UINT h);
    void begin_frame();   // clears to fully transparent
    void end_frame();     // presents

    ID3D11Device* device() const { return device_; }
    ID3D11DeviceContext* context() const { return ctx_; }

private:
    void create_target();
    void release_target();

    ID3D11Device* device_{};
    ID3D11DeviceContext* ctx_{};
    IDXGISwapChain* swap_{};
    ID3D11RenderTargetView* rtv_{};
};

}  // namespace core
