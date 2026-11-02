#pragma once
#include <initializer_list>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Table>.
//   if (w::table_begin("t", {"Name", "Status"})) { w::table_row(); text; w::table_cell(); text; w::table_end(); }
// Call table_end() ONLY when table_begin() returned true. height 0 = fit, >0 = scrollable.
inline bool table_begin(const char* id, std::initializer_list<const char*> columns, float height = 0.f) {
    using namespace theme;
    ImGui::PushStyleColor(ImGuiCol_TableBorderLight, c.border);
    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, c.border);
    ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4{0, 0, 0, 0});
    ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImVec4{0, 0, 0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {m.space_2, m.space_2});
    ImGuiTableFlags fl = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoSavedSettings;
    if (height > 0.f) fl |= ImGuiTableFlags_ScrollY;
    if (ImGui::BeginTable(id, static_cast<int>(columns.size()), fl, {0.f, height})) {
        for (const char* col : columns) ImGui::TableSetupColumn(col);
        ImGui::TableNextRow();
        int i = 0;
        for (const char* col : columns) {
            ImGui::TableSetColumnIndex(i++);
            ImGui::PushFont(fonts::medium, m.font_sm);
            ImGui::PushStyleColor(ImGuiCol_Text, c.muted_foreground);
            ImGui::TextUnformatted(col);
            ImGui::PopStyleColor();
            ImGui::PopFont();
        }
        return true;
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
    return false;
}
inline void table_row() { ImGui::TableNextRow(); ImGui::TableNextColumn(); }
inline void table_cell() { ImGui::TableNextColumn(); }
inline void table_end() { ImGui::EndTable(); ImGui::PopStyleVar(); ImGui::PopStyleColor(4); }

}  // namespace ui::w
