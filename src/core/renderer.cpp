#include "core/renderer.hpp"

namespace core {

bool Renderer::init(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    constexpr D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};
    D3D_FEATURE_LEVEL got{};
    UINT flags = 0;
#ifdef LOADER_DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    auto make = [&](UINT f, D3D_DRIVER_TYPE t) {
        return D3D11CreateDeviceAndSwapChain(nullptr, t, nullptr, f, levels, 2, D3D11_SDK_VERSION, &sd, &swap_,
                                             &device_, &got, &ctx_);
    };
    HRESULT hr = make(flags, D3D_DRIVER_TYPE_HARDWARE);
    if (FAILED(hr) && flags) hr = make(0, D3D_DRIVER_TYPE_HARDWARE);   // no debug layer installed
    if (FAILED(hr)) hr = make(0, D3D_DRIVER_TYPE_WARP);
    if (FAILED(hr)) return false;
    create_target();
    return true;
}

void Renderer::create_target() {
    ID3D11Texture2D* back = nullptr;
    swap_->GetBuffer(0, IID_PPV_ARGS(&back));
    device_->CreateRenderTargetView(back, nullptr, &rtv_);
    back->Release();
}

void Renderer::release_target() {
    if (rtv_) { rtv_->Release(); rtv_ = nullptr; }
}

void Renderer::resize(UINT w, UINT h) {
    if (!swap_ || !w || !h) return;
    release_target();
    swap_->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);
    create_target();
}

void Renderer::begin_frame() {
    constexpr float clear[4] = {0.f, 0.f, 0.f, 0.f};
    ctx_->OMSetRenderTargets(1, &rtv_, nullptr);
    ctx_->ClearRenderTargetView(rtv_, clear);
}

void Renderer::end_frame() { swap_->Present(1, 0); }

void Renderer::shutdown() {
    release_target();
    if (swap_) { swap_->Release(); swap_ = nullptr; }
    if (ctx_) { ctx_->Release(); ctx_ = nullptr; }
    if (device_) { device_->Release(); device_ = nullptr; }
}

}  // namespace core
