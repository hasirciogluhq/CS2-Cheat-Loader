#pragma once
#include "imgui.h"

namespace ui::fonts {

inline ImFont* regular = nullptr;
inline ImFont* medium = nullptr;
inline ImFont* semibold = nullptr;

void load();   // embedded TTFs (xmake bin2c), call after CreateContext

}  // namespace ui::fonts
