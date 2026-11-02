#pragma once

// Internal helpers shared by the providers. Not part of the public API.

#include "inject/injector.hpp"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

namespace inject::detail {

// Process handle wrapper with the access rights both methods need.
class Process_handle {
public:
    Process_handle() = default;
    explicit Process_handle(void* h) noexcept : h_(h) {}
    Process_handle(Process_handle&& o) noexcept : h_(o.h_) { o.h_ = nullptr; }
    Process_handle& operator=(Process_handle&& o) noexcept;
    Process_handle(const Process_handle&) = delete;
    Process_handle& operator=(const Process_handle&) = delete;
    ~Process_handle();

    [[nodiscard]] void* get() const noexcept { return h_; }
    [[nodiscard]] explicit operator bool() const noexcept { return h_ != nullptr; }

private:
    void* h_ = nullptr;
};

// Name -> PID (numeric names are tried as PID first). Also accepts pid directly.
[[nodiscard]] Result<std::uint32_t> resolve_pid(const Target& t);

// Opens the target with the rights needed for memory ops and remote threads.
// Also checks the target is 64-bit.
[[nodiscard]] Result<Process_handle> open_target(std::uint32_t pid);

// Reads a whole file. Fails on empty files.
[[nodiscard]] Result<std::vector<std::byte>> read_file(const std::filesystem::path& p);

// Finds a module in the target by full path (case-insensitive). Returns its base.
[[nodiscard]] Result<Module_handle> find_module(std::uint32_t pid, const std::filesystem::path& full_path);

// Provider factories (implemented in the per-method .cpp files).
[[nodiscard]] std::unique_ptr<Provider> make_load_library_provider(const Load_library_options& o);
[[nodiscard]] std::unique_ptr<Provider> make_manual_map_provider(const Manual_map_options& o);

[[nodiscard]] std::uint32_t last_error() noexcept;

[[nodiscard]] Error_info make_error(Error e, Stage s, std::uint32_t win32 = 0, std::uint32_t detail = 0) noexcept;

} // namespace inject::detail
