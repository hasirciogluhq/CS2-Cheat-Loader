#include "inject/win_util.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <tlhelp32.h>

#include <cstring>
#include <fstream>
#include <iterator>
#include <optional>

namespace inject::detail {

Process_handle& Process_handle::operator=(Process_handle&& o) noexcept {
    if (this != &o) {
        if (h_) CloseHandle(static_cast<HANDLE>(h_));
        h_ = o.h_;
        o.h_ = nullptr;
    }
    return *this;
}

Process_handle::~Process_handle() {
    if (h_) CloseHandle(static_cast<HANDLE>(h_));
}

std::uint32_t last_error() noexcept { return static_cast<std::uint32_t>(GetLastError()); }

Error_info make_error(Error e, Stage s, std::uint32_t win32, std::uint32_t detail) noexcept {
    return Error_info{e, s, win32, detail};
}

Result<std::uint32_t> resolve_pid(const Target& t) {
    if (t.pid != 0) return t.pid;
    if (t.name.empty()) return std::unexpected(make_error(Error::invalid_argument, Stage::resolve));

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
        return std::unexpected(make_error(Error::process_not_found, Stage::resolve, last_error()));

    PROCESSENTRY32W e{};
    e.dwSize = sizeof(e);
    std::uint32_t found = 0;
    if (Process32FirstW(snap, &e)) {
        do {
            if (_wcsicmp(e.szExeFile, t.name.c_str()) == 0) {
                found = e.th32ProcessID;
                break;
            }
        } while (Process32NextW(snap, &e));
    }
    CloseHandle(snap);

    if (!found) return std::unexpected(make_error(Error::process_not_found, Stage::resolve));
    return found;
}

Result<Process_handle> open_target(std::uint32_t pid) {
    HANDLE proc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
                                  PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
                              FALSE, pid);
    if (!proc) return std::unexpected(make_error(Error::process_open_failed, Stage::open, last_error()));

    Process_handle h(proc);
    BOOL wow = FALSE;
    if (!IsWow64Process(proc, &wow))
        return std::unexpected(make_error(Error::process_open_failed, Stage::open, last_error()));
    if (wow) return std::unexpected(make_error(Error::architecture_mismatch, Stage::open));
    return h;
}

Result<std::vector<std::byte>> read_file(const std::filesystem::path& p) {
    std::error_code ec;
    if (!std::filesystem::exists(p, ec))
        return std::unexpected(make_error(Error::file_not_found, Stage::validate));

    std::ifstream f(p, std::ios::binary);
    if (!f) return std::unexpected(make_error(Error::file_read_failed, Stage::validate));

    std::vector<char> raw((std::istreambuf_iterator<char>(f)), {});
    if (raw.empty()) return std::unexpected(make_error(Error::file_read_failed, Stage::validate));

    std::vector<std::byte> data(raw.size());
    std::memcpy(data.data(), raw.data(), raw.size());
    return data;
}

Result<Module_handle> find_module(std::uint32_t pid, const std::filesystem::path& full_path) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snap == INVALID_HANDLE_VALUE)
        return std::unexpected(make_error(Error::module_not_found, Stage::verify, last_error()));

    MODULEENTRY32W m{};
    m.dwSize = sizeof(m);
    std::optional<Module_handle> found;
    const std::wstring want = full_path.wstring();
    if (Module32FirstW(snap, &m)) {
        do {
            if (_wcsicmp(m.szExePath, want.c_str()) == 0) {
                found = Module_handle{reinterpret_cast<std::uintptr_t>(m.modBaseAddr), m.modBaseSize};
                break;
            }
        } while (Module32NextW(snap, &m));
    }
    CloseHandle(snap);

    if (!found) return std::unexpected(make_error(Error::module_not_found, Stage::verify));
    return *found;
}

} // namespace inject::detail
