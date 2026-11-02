# CS2 Loader (educational project)

An educational Windows project that shows how a native C++26 application is built
end to end: a Win32 window with acrylic blur, a DirectX 11 renderer driven by Dear ImGui,
a small design-token UI library, and an injection API that demonstrates two ways of
loading code into another process.

> **Disclaimer.** This project is for learning and for use on software you own or are
> authorized to test. It contains no game-specific logic and no anti-detection or
> evasion features, and none will be accepted. Do not use it to violate any game's
> terms of service or any law.

## What is in here

| Area | Path | What it shows |
|------|------|---------------|
| Window | `src/core/window.*` | Borderless Win32 window, acrylic blur, rounded corners, dragging |
| Renderer | `src/core/renderer.*` | DX11 device, swap chain, ImGui backend |
| UI kit | `src/ui/` | Design tokens (zinc dark), primitives and ~30 shadcn-style widgets on ImGui |
| Injection API | `src/inject/` | Provider abstraction with two methods, each with its own options (see below) |

The injection API is **not wired into the UI**. It is a standalone library that you
can read, build and test on its own.

### Injection API

`inject::Options` is a `std::variant` of two methods. Each method has its own options:

- `Load_library_options`: writes the module path into the target and runs
  `LoadLibraryW` in a remote thread. Options: `timeout_ms`, `verify_loaded`.
- `Manual_map_options`: maps a PE image into the target without the Windows loader.
  Options: `timeout_ms`, `prefer_image_base`, `adjust_protections`, `enable_seh`,
  `enable_tls_callbacks`, `call_entry`, `dll_reason`, `reserved`.

Input is a file (both methods) or a memory buffer (manual map only).

Every error carries a `Stage` (for example `open`, `write`, `imports`, `entry`) and a
Win32 code or stage detail where available, so a failure tells you where it happened.

Scope and limits of the manual mapper:
- Windows x64 only, targets must be 64-bit.
- Static TLS data and C++ throw trampolines (`_CxxThrowException` / `RtlPcToFileHeader`)
  are not implemented.
- Only the mapping and initialization path is implemented. There is no unload path.

Header wiping, section stripping and module hiding are intentionally left out.

## Language and toolchain rules

- **C++26 only** (`set_languages("c++26")`). Code must not target C++23 or older standards.
- C++23 language and library features are allowed when they are part of C++26.
- Write modern code: `std::expected` for errors, `std::print`, `std::span`, RAII, `enum class`,
  no raw `new`/`delete`, no C-style casts, no macros unless the language offers no alternative.
- Windows x64 is the only platform. The code does not build for other targets.

See [CLAUDE.md](CLAUDE.md) for the full rules used by contributors and AI assistants.

## Build

Requirements: [xmake](https://xmake.io).

### macOS / Linux host, MinGW cross-compile (tested)

```sh
brew install xmake mingw-w64
xmake f -y -p mingw -a x86_64 -m debug --mingw=/opt/homebrew   # or -m release
xmake
# or: ./build.sh debug
```

Requires GCC 14 or newer (for `<print>` and C++26 support). The binary is statically
linked. Run the `.exe` with Wine/CrossOver or on a Windows machine.

### Windows, MSVC (not tested yet)

Requires Visual Studio 2022 or newer with the C++ workload.

```powershell
xmake f -y -p windows -a x64 -m release   # or -m debug
xmake
.\out\release\windows-x64\CS2Loader.exe
```

The MSVC configuration is written but has not been tested. Reports and fixes are welcome.

### Output

- `out/<mode>/<os>-<arch>/CS2Loader.exe`, for example `out/debug/windows-x86_64/`
- Intermediates go to `build/`

## Project layout

```
src/
  main.cpp            entry point
  core/               window, renderer, config
  ui/                 theme, style, fonts, primitives, widgets
  inject/             injection API (Load_library and Manual_map providers)
assets/fonts/         Open Sans, embedded at build time with xmake's bin2c rule
xmake.lua             build description (ImGui is fetched automatically)
build.sh              convenience wrapper around xmake
```

## Third-party software

- [Dear ImGui](https://github.com/ocornut/imgui), MIT license, fetched by xmake
- [Open Sans](https://fonts.google.com/specimen/Open+Sans), SIL Open Font License 1.1

## Contributing

Read [CONTRIBUTING.md](CONTRIBUTING.md) before opening a pull request, and follow the
[Code of Conduct](CODE_OF_CONDUCT.md). Report security issues as described in
[SECURITY.md](SECURITY.md).

## License

Apache License 2.0. See [LICENSE](LICENSE).
