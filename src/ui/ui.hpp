#pragma once
#include "core/renderer.hpp"
#include "core/window.hpp"

namespace ui {

bool init(core::Window& win, core::Renderer& rend);
void shutdown();
void frame(core::Window& win);   // builds + submits one ImGui frame (between begin/end of renderer)

}  // namespace ui
