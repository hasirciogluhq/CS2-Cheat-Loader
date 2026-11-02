// LoadLibrary method: CreateRemoteThread(LoadLibraryW, path). The Windows loader
// does the work; this only places the path in the target and starts the thread.

#include "inject/win_util.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <memory>

namespace inject::detail {
namespace {

class Load_library_provider final : public Provider {
public:
    explicit Load_library_provider(Load_library_options o) : opt_(o) {}

    [[nodiscard]] Method method() const noexcept override { return Method::load_library; }

    [[nodiscard]] Result<Module_handle> inject(const Target& target, const Module_source& src) override {
        if (src.kind != Module_source::Kind::file)
            return std::unexpected(make_error(Error::unsupported, Stage::validate));

        std::error_code ec;
        if (!std::filesystem::exists(src.path, ec))
            return std::unexpected(make_error(Error::file_not_found, Stage::validate));
        const std::filesystem::path full = std::filesystem::absolute(src.path, ec);
        if (ec) return std::unexpected(make_error(Error::file_not_found, Stage::validate));

        auto pid = resolve_pid(target);
        if (!pid) return std::unexpected(pid.error());

        auto proc = open_target(*pid);
        if (!proc) return std::unexpected(proc.error());
        HANDLE h = static_cast<HANDLE>(proc->get());

        const std::wstring path = full.wstring();
        const SIZE_T bytes = (path.size() + 1) * sizeof(wchar_t);

        void* remote = VirtualAllocEx(h, nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!remote) return std::unexpected(make_error(Error::alloc_failed, Stage::allocate, last_error()));

        if (!WriteProcessMemory(h, remote, path.c_str(), bytes, nullptr)) {
            auto err = last_error();
            VirtualFreeEx(h, remote, 0, MEM_RELEASE);
            return std::unexpected(make_error(Error::write_failed, Stage::write, err));
        }

        // kernel32 is mapped at the same address in all processes of one bitness per boot.
        auto load = reinterpret_cast<LPTHREAD_START_ROUTINE>(
            reinterpret_cast<std::uintptr_t>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW")));
        HANDLE thread = CreateRemoteThread(h, nullptr, 0, load, remote, 0, nullptr);
        if (!thread) {
            auto err = last_error();
            VirtualFreeEx(h, remote, 0, MEM_RELEASE);
            return std::unexpected(make_error(Error::remote_thread_failed, Stage::remote, err));
        }

        DWORD wait = WaitForSingleObject(thread, opt_.timeout_ms);
        if (wait == WAIT_TIMEOUT) {
            // The thread may still be inside LoadLibrary, so the path buffer is not freed here.
            CloseHandle(thread);
            return std::unexpected(make_error(Error::timeout, Stage::remote));
        }
        if (wait != WAIT_OBJECT_0) {
            auto err = last_error();
            CloseHandle(thread);
            VirtualFreeEx(h, remote, 0, MEM_RELEASE);
            return std::unexpected(make_error(Error::remote_thread_failed, Stage::remote, err));
        }
        DWORD exit_code = 0;
        GetExitCodeThread(thread, &exit_code);
        CloseHandle(thread);
        VirtualFreeEx(h, remote, 0, MEM_RELEASE);

        // LoadLibraryW returns HMODULE; the exit code is only 32 bits, so it is not used as the base.
        // A zero result means the target's loader could not load the file (bad path, missing
        // dependency, DllMain returned FALSE). The target-side error code is not available here.
        if (exit_code == 0) return std::unexpected(make_error(Error::module_not_found, Stage::remote));

        if (!opt_.verify_loaded) return Module_handle{};
        auto mod = find_module(*pid, full);
        if (!mod) return std::unexpected(mod.error());
        return *mod;
    }

private:
    Load_library_options opt_;
};

} // namespace

std::unique_ptr<Provider> make_load_library_provider(const Load_library_options& o) {
    return std::make_unique<Load_library_provider>(o);
}

} // namespace inject::detail
