#pragma once

// Injection system (Windows x64 only).
//
// Two methods, each with its own options:
//   - Load_library_options : CreateRemoteThread(LoadLibraryW, path)
//   - Manual_map_options   : maps the PE image into the target and runs it
//                            through a small in-target bootstrap (no loader).
//
// inject() picks the method from the variant index of Options. Errors carry the
// stage where they happened, so callers can tell "could not open process" from
// "import 37 did not resolve".

#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#if !defined(_WIN64)
#error "inject: x64 only"
#endif

namespace inject {

enum class Method : std::uint8_t { load_library, manual_map };

enum class Stage : std::uint8_t {
    none,
    validate,       // inputs and options
    resolve,        // process name -> PID
    open,           // OpenProcess / IsWow64Process
    allocate,       // VirtualAllocEx
    write,          // WriteProcessMemory
    remote,         // CreateRemoteThread / wait
    verify,         // post-load checks (module lookup)
    relocate,       // base relocations (inside the target bootstrap)
    imports,        // import resolution (inside the target bootstrap)
    protect,        // section protections (inside the target bootstrap)
    seh,            // RtlAddFunctionTable (inside the target bootstrap)
    tls,            // TLS callbacks (inside the target bootstrap)
    entry,          // DllMain (inside the target bootstrap)
};

enum class Error : std::uint8_t {
    invalid_argument,
    invalid_options,
    process_not_found,
    process_open_failed,
    architecture_mismatch,
    file_not_found,
    file_read_failed,
    invalid_image,
    unsupported,
    relocation_required,   // image cannot be placed at its preferred base and has no relocs
    alloc_failed,
    write_failed,
    protect_failed,
    remote_thread_failed,
    timeout,
    module_not_found,      // LoadLibrary returned, but the module is not in the target
    import_failed,
    seh_failed,
    entry_failed,          // DllMain returned FALSE
    internal,
};

struct Error_info {
    Error code{};
    Stage stage{Stage::none};
    std::uint32_t win32 = 0;     // GetLastError() when relevant, else 0
    std::uint32_t detail = 0;    // stage-specific (e.g. failing import index)
};

template <class T>
using Result = std::expected<T, Error_info>;

[[nodiscard]] std::string_view to_string(Error e) noexcept;
[[nodiscard]] std::string_view to_string(Stage s) noexcept;

// Process to inject into. Set pid, or name (e.g. L"notepad.exe"). If both are
// set, pid wins.
struct Target {
    std::uint32_t pid = 0;
    std::wstring name;
};

// Module source. File is supported by both methods; Buffer only by manual map.
struct Module_source {
    enum class Kind : std::uint8_t { file, buffer };

    Kind kind = Kind::file;
    std::filesystem::path path;        // Kind::file
    std::span<const std::byte> bytes;  // Kind::buffer; must outlive the call

    [[nodiscard]] static Module_source from_file(std::filesystem::path p) {
        Module_source s;
        s.kind = Kind::file;
        s.path = std::move(p);
        return s;
    }
    [[nodiscard]] static Module_source from_buffer(std::span<const std::byte> b) {
        Module_source s;
        s.kind = Kind::buffer;
        s.bytes = b;
        return s;
    }
};

// Result of a successful injection. base is the module base in the target.
struct Module_handle {
    std::uintptr_t base = 0;
    std::uint32_t image_size = 0;   // manual map only; 0 for LoadLibrary
};

struct Load_library_options {
    std::uint32_t timeout_ms = 30'000;
    // After LoadLibrary returns, look the module up in the target's module list
    // to get its full base (the thread exit code is only 32 bits).
    bool verify_loaded = true;
};

struct Manual_map_options {
    std::uint32_t timeout_ms = 30'000;
    // Try the PE's preferred ImageBase first. Skips relocations when it works.
    bool prefer_image_base = true;
    // Apply section protections from the PE header (RW/RX/RO/RWX) instead of RWX.
    bool adjust_protections = true;
    // Register .pdata so C++/SEH unwinding works inside the mapped image.
    bool enable_seh = true;
    // Run TLS callbacks (DLL_PROCESS_ATTACH only) before DllMain.
    bool enable_tls_callbacks = true;
    // Call the entry point. If false, the image is mapped and relocated but DllMain is not run.
    bool call_entry = true;
    std::uint32_t dll_reason = 1;     // DLL_PROCESS_ATTACH
    std::uint64_t reserved = 0;       // DllMain lpReserved
};

using Options = std::variant<Load_library_options, Manual_map_options>;

[[nodiscard]] Method method_of(const Options& o) noexcept;
[[nodiscard]] Result<void> validate(const Options& o);

// Provider: one injection method bound to its options.
class Provider {
public:
    virtual ~Provider() = default;
    [[nodiscard]] virtual Method method() const noexcept = 0;
    [[nodiscard]] virtual Result<Module_handle> inject(const Target& target,
                                                       const Module_source& src) = 0;
};

[[nodiscard]] Result<std::unique_ptr<Provider>> make_provider(const Options& opts);

// One-shot helper: validates options, builds the provider, injects.
[[nodiscard]] Result<Module_handle> inject(const Target& target,
                                           const Module_source& src,
                                           const Options& opts = Manual_map_options{});

} // namespace inject
