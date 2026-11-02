# CS2 Loader

Empty loader template: native Windows 10/11 window, acrylic blur background, DX11 + ImGui
with rounded, almost transparent window background. C++26, built with xmake.
ImGui is downloaded automatically by xmake (no manual download).

## Design decisions

- Native Win32 + DX11 (no SDL): reliable acrylic blur + per-pixel alpha, nothing extra to build.
- C++26 (`std::print`), no modules/`import std` (MinGW module support is still unstable).
- No `imgui.ini` is ever written (`IniFilename = nullptr`).
- Fonts (Open Sans) are embedded at build time with xmake's `utils.bin2c` rule (`assets/fonts/*.ttf`).
- Theme follows shadcn/ui zinc dark (`src/ui/theme.hpp        design tokens (index.css equivalent): colors, radius, spacing, font sizes
src/ui/style.*          pushes tokens into ImGuiStyle (call apply_style() after changing a token)
src/ui/fonts.*          embedded font loading
src/ui/primitives/      text (h1-h4/text/paragraph/label/muted/small), layout (spacer/same_line/column_width),
                        separator/vseparator, spinner
src/ui/widgets/         button (5 variants, 3 sizes), card, badge, kbd, input, checkbox, toggle (switch),
                        slider, progress, tabs, select, alert, avatar, tooltip, textarea, radio_group, toggle_group,
                        accordion, popup (menu/context menu/popover/dialog), skeleton, breadcrumb, pagination,
                        table, scroll_area, toast, combobox, number_input
                        (#include "ui/widgets/widgets.hpp" for all of them)
src/ui/ui.*             ImGui init + per-frame UI (put loader logic here)
assets/fonts/           embedded TTFs
```

Output: `out/<mode>/<os>-<arch>/CS2Loader.exe` (e.g. `out/debug/windows-x86_64/`).
Intermediates: `build/`.

## Build on Windows (MSVC) — NOT TESTED

> The MSVC config is written but has not been tested yet.

Requirements: Visual Studio 2022+ (C++ workload, `/std:c++latest`), xmake.

```powershell
xmake f -y -p windows -a x64 -m release   # or -m debug
xmake
# or simply: ./build.sh debug
.\out\release\windows-x86_64\CS2Loader.exe
```

Note: on MSVC the arch folder is `x64` (`out/release/windows-x64/`).

## Build on macOS (MinGW cross-compile, tested)

```sh
brew install xmake mingw-w64
xmake f -y -p mingw -a x86_64 -m debug --mingw=/opt/homebrew   # or -m release
xmake
# or simply: ./build.sh debug
# run the .exe with Wine/CrossOver (or copy it to a Windows machine)
```

Requires GCC 14+ (for `<print>`, `-std=c++26`). The binary is statically linked.
Debug builds open a console and use the D3D11 debug layer when available.
Blur/transparency under Wine/CrossOver may look different than on real Windows.

## How to test the MSVC build (with Claude)

On a Windows machine, open Claude Code in this repo and use this prompt:

```
Build this project on Windows with MSVC using xmake:
  xmake f -y -p windows -a x64 -m debug && xmake
then the same for -m release. Fix every compile/link error or warning you find
(the MSVC config in xmake.lua has never been tested), keep C++26 (/std:c++latest),
keep the out/<mode>/<os>-<arch> layout, and make sure the MinGW build still works.
Run the exe and verify: borderless window, acrylic blur, rounded corners,
draggable, close button works. Report what you changed.
```
