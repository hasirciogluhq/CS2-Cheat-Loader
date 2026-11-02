set_project("CS2Loader")
set_version("0.1.0")
set_xmakever("2.9.0")

add_rules("mode.debug", "mode.release")
set_config("builddir", "build")   -- intermediates (objects, deps)

-- Output layout: out/<mode>/<os>-<arch>/CS2Loader.exe
-- (mingw cross-build and MSVC both report "windows" as the OS)
local os_name = (is_plat("mingw") or is_plat("windows")) and "windows" or "$(plat)"
local out_dir = path.join("out", "$(mode)", os_name .. "-$(arch)")
set_languages("c++26")
set_warnings("all")

-- imgui is downloaded + built by xmake (DX11 + Win32 backends only)
add_requires("imgui", {configs = {dx11 = true, win32 = true}})

target("CS2Loader")
    set_kind("binary")
    set_basename("CS2Loader")
    set_targetdir(out_dir)
    add_files("src/**.cpp")
    -- native xmake embedding: assets/fonts/*.ttf -> generated "<name>.ttf.h" byte arrays
    add_rules("utils.bin2c", {extensions = ".ttf"})
    add_files("assets/fonts/*.ttf")
    add_includedirs("src")
    add_packages("imgui")
    add_syslinks("user32", "gdi32", "dwmapi", "d3d11", "dxgi", "d3dcompiler", "ole32")
    add_defines("UNICODE", "_UNICODE", "WIN32_LEAN_AND_MEAN", "NOMINMAX")

    if is_mode("debug") then
        add_defines("LOADER_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    else
        add_defines("NDEBUG")
        set_symbols("hidden")
        set_optimize("fastest")
        set_strip("all")
    end

    if is_plat("mingw") then
        add_ldflags("-mwindows", "-static", "-static-libgcc", "-static-libstdc++", "-lstdc++exp", {force = true})
    elseif is_plat("windows") then
        add_ldflags("/SUBSYSTEM:WINDOWS", "/ENTRY:mainCRTStartup", {force = true})
        add_cxflags("/utf-8", "/permissive-", "/Zc:__cplusplus", {force = true})
    end
