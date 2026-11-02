#pragma once
#include <algorithm>
#include <string>
#include <vector>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn/sonner-style toasts. Call toast(...) anywhere; call toast_render() once per frame
// (after your UI, before ImGui::Render()).
struct Toast { std::string title, description; double born; float duration; bool destructive; };
inline std::vector<Toast>& toast_queue() { static std::vector<Toast> q; return q; }

inline void toast(std::string title, std::string description = {}, float seconds = 3.f, bool destructive = false) {
    toast_queue().push_back({std::move(title), std::move(description), ImGui::GetTime(), seconds, destructive});
}

inline void toast_render() {
    using namespace theme;
    auto& q = toast_queue();
    const double now = ImGui::GetTime();
    std::erase_if(q, [&](const Toast& t) { return now - t.born > t.duration; });
    if (q.empty()) return;

    const ImGuiViewport* vp = ImGui::GetMainViewport();
    auto* dl = ImGui::GetForegroundDrawList();
    const float w = 280.f, pad = m.space_3;
    float y = vp->Pos.y + vp->Size.y - m.space_4;
    for (auto it = q.rbegin(); it != q.rend(); ++it) {
        const float age = static_cast<float>(now - it->born), left = it->duration - age;
        const float alpha = std::min({1.f, age / 0.15f, left / 0.3f});
        ImGui::PushFont(fonts::regular, m.font_sm);
        const float dh = it->description.empty() ? 0.f
            : ImGui::CalcTextSize(it->description.c_str(), nullptr, false, w - pad * 2.f).y + 2.f;
        ImGui::PopFont();
        const float h = pad * 2.f + m.font_base + 2.f + dh;
        const ImVec2 a{vp->Pos.x + vp->Size.x - m.space_4 - w, y - h}, b{a.x + w, y};
        dl->AddRectFilled(a, b, ImGui::GetColorU32(mul_alpha(c.popover, alpha)), m.radius_lg() - 2.f);
        dl->AddRect(a, b, ImGui::GetColorU32(mul_alpha(it->destructive ? with_alpha(c.destructive, 0.7f) : c.border, alpha)), m.radius_lg() - 2.f);
        dl->AddText(fonts::semibold, m.font_base, {a.x + pad, a.y + pad},
                    ImGui::GetColorU32(mul_alpha(it->destructive ? c.destructive : c.popover_foreground, alpha)), it->title.c_str());
        if (dh > 0.f)
            dl->AddText(fonts::regular, m.font_sm, {a.x + pad, a.y + pad + m.font_base + 4.f},
                        ImGui::GetColorU32(mul_alpha(c.muted_foreground, alpha)), it->description.c_str(), nullptr, w - pad * 2.f);
        y -= h + m.space_2;
    }
}

}  // namespace ui::w
