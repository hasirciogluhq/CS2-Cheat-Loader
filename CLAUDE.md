# CLAUDE.md

Guidance for Claude Code and other AI assistants working in this repository.

## What this project is

An educational Windows C++26 project: a native Win32/DX11/ImGui application and a
standalone injection API with two methods (LoadLibrary and manual map). It exists to
teach how these pieces work. It has no game-specific logic and must stay that way.

## Hard rules

1. **C++26 only.** Target `c++26` (`set_languages("c++26")`). Do not write code that
   targets C++23 or older, and do not add flags or guards that switch to older standards.
   C++23 language and library features are allowed because they are part of C++26.
2. **Next-generation code.** Use modern C++26 idioms: `std::expected` for error
   handling, `std::print`, `std::span`, `std::string_view`, `enum class`, RAII, `[[nodiscard]]`,
   `constexpr`/`consteval` where they fit. Do not write C-style code: no raw `new`/`delete`,
   no C-style casts, no `malloc`, no unnecessary macros.
3. **Windows x64 only.** Do not add other platforms. The inject API is `#error`-guarded
   to `_WIN64`.
4. **No evasion.** Do not add anti-detection, header wiping, section stripping, module
   hiding, unlinking, or similar features. Decline if asked, and say why.
5. **Injection stays standalone.** Do not connect `src/inject/` to `main.cpp` or the UI
   unless the user explicitly asks for that change.
6. **Educational framing.** Comments and docs explain what code does and why. Keep the
   README disclaimer accurate.

## Build and verify

- Full build: `./build.sh debug` (MinGW cross-compile on macOS, output in
  `out/debug/windows-x86_64/CS2Loader.exe`). This must pass before any commit.
- Quick compile check of one file:
  `x86_64-w64-mingw32-g++ -std=c++26 -O2 -c -Wall -Wextra -Isrc <file.cpp> -o /dev/null`
- The host clangd diagnostics under `src/inject/` are noise (no MinGW include paths).
  Use the MinGW compile as the real check.
- MSVC config is untested. Do not claim it works without running it on Windows.
- Runtime injection cannot be tested on macOS. Say so when reporting results.

## Repository conventions

- Commits: short imperative subject, one logical change per commit.
- Branches: do not commit directly to `main`. Use `feat/...`, `fix/...`, `docs/...`, `chore/...`.
- Follow [CONTRIBUTING.md](CONTRIBUTING.md) and the [Code of Conduct](CODE_OF_CONDUCT.md).
- New source files: `.cpp`/`.hpp`, four-space indent, LF line endings (see `.editorconfig`).
- Do not commit generated output (`out/`, `build/`, `.xmake/`).

## Layout

- `src/core/`: window, renderer, config
- `src/ui/`: theme tokens, style, fonts, primitives, widgets
- `src/inject/`: `injector.hpp` (public API), `win_util.*` (internal helpers),
  `load_library_provider.cpp`, `manual_map_provider.cpp`, `provider_factory.cpp`
- `xmake.lua`: build; `build.sh`: wrapper
