// Manual-map method (x64 host, x64 target).
//
// Steps:
//   host : validate PE -> allocate image in target -> write headers + sections
//          -> write the in-target bootstrap and its parameter block -> run the bootstrap
//   bootstrap : apply base relocations -> resolve imports -> set section protections
//          -> register .pdata -> run TLS callbacks -> call DllMain
//   host : read the bootstrap's result; on failure free the image
//
// The bootstrap is copied from this process into the target and runs there. It must be
// self-contained: no direct calls, no string literals, no globals, and no CRT
// calls the optimizer may insert. Everything it needs comes through Bootstrap_params.
// The provider checks the bootstrap size and that it makes no direct calls.

#include "inject/win_util.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <algorithm>
#include <cstring>
#include <memory>
#include <span>
#include <vector>

namespace inject::detail {
namespace {

// Bootstrap flags (Bootstrap_params::flags).
constexpr std::uint32_t kFlagAdjustProtections = 1u << 0;
constexpr std::uint32_t kFlagEnableSeh         = 1u << 1;
constexpr std::uint32_t kFlagTlsCallbacks      = 1u << 2;
constexpr std::uint32_t kFlagCallEntry         = 1u << 3;

// Layout shared between host and bootstrap (both x64, fixed-width fields).
struct Bootstrap_params {
    std::uint64_t load_library_a;          // target's LoadLibraryA
    std::uint64_t get_proc_address;        // target's GetProcAddress
    std::uint64_t virtual_protect;         // target's VirtualProtect
    std::uint64_t rtl_add_function_table;  // target's RtlAddFunctionTable
    std::uint64_t rtl_delete_function_table;
    std::uint64_t image_base;              // remote base of the mapped image
    std::uint64_t reserved;                // DllMain lpReserved
    std::uint32_t reason;                  // DllMain fdwReason
    std::uint32_t flags;                   // kFlag*
    std::uint32_t fail_stage;              // out: Stage value, 0 = no failure
    std::uint32_t fail_detail;             // out: stage-specific detail
    std::uint32_t result;                  // out: 1 = success
    std::uint32_t pad;
};
static_assert(sizeof(Bootstrap_params) == 80);

#if defined(__clang__)
#define MM_BOOTSTRAP_ATTR __attribute__((noinline, no_stack_protector))
#elif defined(__GNUC__)
#define MM_BOOTSTRAP_ATTR \
    __attribute__((noinline, optimize("no-stack-protector", "no-tree-loop-distribute-patterns")))
#else
#define MM_BOOTSTRAP_ATTR __declspec(noinline)
#pragma optimize("", off)
#pragma strict_gs_check(push, off)
#endif

MM_BOOTSTRAP_ATTR
std::uint32_t manual_map_bootstrap(Bootstrap_params* d) {
    using load_fn = void* (*)(const char*);
    using proc_fn = void* (*)(void*, const char*);
    using protect_fn = int (*)(void*, std::uint64_t, std::uint32_t, std::uint32_t*);
    using rtl_add_fn = unsigned char (*)(void*, std::uint32_t, std::uint64_t);
    using rtl_del_fn = unsigned char (*)(void*);
    using entry_fn = int (*)(void*, std::uint32_t, void*);
    using tls_fn = void (*)(void*, std::uint32_t, void*);

    d->result = 0;
    d->fail_stage = 0;
    d->fail_detail = 0;

    auto* base = reinterpret_cast<std::uint8_t*>(d->image_base);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    IMAGE_OPTIONAL_HEADER64& opt = nt->OptionalHeader;

    auto load = reinterpret_cast<load_fn>(d->load_library_a);
    auto get_proc = reinterpret_cast<proc_fn>(d->get_proc_address);
    auto virt_protect = reinterpret_cast<protect_fn>(d->virtual_protect);
    auto rtl_add = reinterpret_cast<rtl_add_fn>(d->rtl_add_function_table);
    auto rtl_del = reinterpret_cast<rtl_del_fn>(d->rtl_delete_function_table);

    // Set when .pdata was registered, so a later failure can unregister it.
    void* seh_table = nullptr;

#define MM_FAIL(stage, detail)                                              \
    do {                                                                    \
        d->fail_stage = static_cast<std::uint32_t>(stage);                  \
        d->fail_detail = static_cast<std::uint32_t>(detail);                \
        if (seh_table && rtl_del) rtl_del(seh_table);                       \
        return 0;                                                           \
    } while (0)

    // 1. Base relocations (x64: DIR64 entries only).
    const std::uint64_t delta = d->image_base - opt.ImageBase;
    if (delta) {
        const IMAGE_DATA_DIRECTORY& dir = opt.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
        if (dir.Size) {
            auto* blk = reinterpret_cast<IMAGE_BASE_RELOCATION*>(base + dir.VirtualAddress);
            std::uint8_t* end = reinterpret_cast<std::uint8_t*>(blk) + dir.Size;
            while (reinterpret_cast<std::uint8_t*>(blk) < end &&
                   blk->SizeOfBlock >= sizeof(IMAGE_BASE_RELOCATION)) {
                const std::uint32_t count =
                    (blk->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(std::uint16_t);
                auto* entries = reinterpret_cast<std::uint16_t*>(blk + 1);
                for (std::uint32_t i = 0; i < count; ++i) {
                    const std::uint16_t e = entries[i];
                    const std::uint32_t type = e >> 12;
                    if (type == IMAGE_REL_BASED_ABSOLUTE) continue;
                    if (type != IMAGE_REL_BASED_DIR64) MM_FAIL(8 /* relocate */, type);
                    auto* p = reinterpret_cast<std::uint64_t*>(base + blk->VirtualAddress + (e & 0xFFF));
                    *p += delta;
                }
                blk = reinterpret_cast<IMAGE_BASE_RELOCATION*>(
                    reinterpret_cast<std::uint8_t*>(blk) + blk->SizeOfBlock);
            }
        }
    }

    // 2. Imports. detail = descriptor index * 0x10000 + thunk index.
    const IMAGE_DATA_DIRECTORY& imp = opt.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (imp.Size) {
        auto* desc = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base + imp.VirtualAddress);
        std::uint32_t desc_index = 0;
        for (; desc->Name; ++desc, ++desc_index) {
            void* mod = load(reinterpret_cast<const char*>(base + desc->Name));
            if (!mod) MM_FAIL(9 /* imports */, desc_index * 0x10000u);

            auto* iat = reinterpret_cast<std::uint64_t*>(base + desc->FirstThunk);
            auto* ilt = reinterpret_cast<std::uint64_t*>(
                base + (desc->OriginalFirstThunk ? desc->OriginalFirstThunk : desc->FirstThunk));
            std::uint32_t thunk_index = 0;
            for (; *ilt; ++ilt, ++iat, ++thunk_index) {
                void* fn = nullptr;
                if (*ilt & (static_cast<std::uint64_t>(1) << 63)) {
                    fn = get_proc(mod, reinterpret_cast<const char*>(*ilt & 0xFFFF));
                } else {
                    auto* byname = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(base + (*ilt & 0xFFFFFFFF));
                    fn = get_proc(mod, byname->Name);
                }
                if (!fn) MM_FAIL(9 /* imports */, desc_index * 0x10000u + thunk_index);
                *iat = reinterpret_cast<std::uint64_t>(fn);
            }
        }
    }

    // 3. Section protections (only when the host enabled them).
    if (d->flags & kFlagAdjustProtections) {
        std::uint32_t old = 0;
        // Headers are read-only.
        if (!virt_protect(base, opt.SizeOfHeaders, PAGE_READONLY, &old))
            MM_FAIL(10 /* protect */, 0);

        const IMAGE_NT_HEADERS64* hdr = nt;
        const auto* sec = reinterpret_cast<const IMAGE_SECTION_HEADER*>(
            reinterpret_cast<const std::uint8_t*>(&hdr->OptionalHeader) + hdr->FileHeader.SizeOfOptionalHeader);
        for (std::uint32_t i = 0; i < hdr->FileHeader.NumberOfSections; ++i, ++sec) {
            const std::uint32_t vsize = sec->Misc.VirtualSize ? sec->Misc.VirtualSize : sec->SizeOfRawData;
            if (!vsize) continue;
            const std::uint32_t ch = sec->Characteristics;
            const bool x = (ch & IMAGE_SCN_MEM_EXECUTE) != 0;
            const bool w = (ch & IMAGE_SCN_MEM_WRITE) != 0;
            std::uint32_t prot = PAGE_READONLY;
            if (x && w) prot = PAGE_EXECUTE_READWRITE;
            else if (x) prot = PAGE_EXECUTE_READ;
            else if (w) prot = PAGE_READWRITE;
            if (!virt_protect(base + sec->VirtualAddress, vsize, prot, &old))
                MM_FAIL(10 /* protect */, sec->VirtualAddress);
        }
    }

    // 4. Exception directory -> RtlAddFunctionTable (x64 SEH/C++ unwinding).
    if (d->flags & kFlagEnableSeh) {
        const IMAGE_DATA_DIRECTORY& exc = opt.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
        if (exc.Size) {
            void* table = base + exc.VirtualAddress;
            const std::uint32_t count = exc.Size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY);
            if (!rtl_add(table, count, d->image_base)) MM_FAIL(11 /* seh */, exc.VirtualAddress);
            seh_table = table;
        }
    }

    // 5. TLS callbacks (run only for DLL_PROCESS_ATTACH, before DllMain).
    if ((d->flags & kFlagTlsCallbacks) && d->reason == 1 /* DLL_PROCESS_ATTACH */) {
        const IMAGE_DATA_DIRECTORY& tlsd = opt.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
        if (tlsd.Size) {
            auto* tls = reinterpret_cast<IMAGE_TLS_DIRECTORY64*>(base + tlsd.VirtualAddress);
            auto* cb = reinterpret_cast<tls_fn*>(tls->AddressOfCallBacks);
            if (cb) {
                for (; *cb; ++cb) (*cb)(base, 1 /* DLL_PROCESS_ATTACH */, nullptr);
            }
        }
    }

    // 6. DllMain.
    if ((d->flags & kFlagCallEntry) && opt.AddressOfEntryPoint) {
        auto entry = reinterpret_cast<entry_fn>(base + opt.AddressOfEntryPoint);
        const int ok = entry(base, d->reason, reinterpret_cast<void*>(d->reserved));
        if (!ok && d->reason == 1 /* DLL_PROCESS_ATTACH */) MM_FAIL(13 /* entry */, 0);
    }

    d->result = 1;
    return 0;
#undef MM_FAIL
}

MM_BOOTSTRAP_ATTR
void manual_map_bootstrap_end() {}

#if !defined(__clang__) && !defined(__GNUC__)
#pragma strict_gs_check(pop)
#pragma optimize("", on)
#endif

// Bootstrap byte size, or 0 if the layout looks wrong (bootstrap and end marker not in one block).
std::size_t bootstrap_size() {
    auto a = reinterpret_cast<std::uintptr_t>(&manual_map_bootstrap);
    auto b = reinterpret_cast<std::uintptr_t>(&manual_map_bootstrap_end);
    if (b <= a || b - a > 0x1000) return 0;
    return static_cast<std::size_t>(b - a);
}

// ---------------------------------------------------------------------------
// Host side
// ---------------------------------------------------------------------------

struct Pe_view {
    const std::uint8_t* bytes = nullptr;
    std::size_t size = 0;
    const IMAGE_NT_HEADERS64* nt = nullptr;
    const IMAGE_SECTION_HEADER* sections = nullptr;
    std::uint32_t section_count = 0;
};

Error_info invalid_image(std::uint32_t detail = 0) {
    return make_error(Error::invalid_image, Stage::validate, 0, detail);
}

Result<Pe_view> validate_pe(std::span<const std::byte> img) {
    Pe_view v;
    v.bytes = reinterpret_cast<const std::uint8_t*>(img.data());
    v.size = img.size();

    if (v.size < sizeof(IMAGE_DOS_HEADER)) return std::unexpected(invalid_image(1));
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(v.bytes);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return std::unexpected(invalid_image(2));
    if (dos->e_lfanew <= 0) return std::unexpected(invalid_image(3));

    const auto lfanew = static_cast<std::uint64_t>(dos->e_lfanew);
    if (lfanew > v.size || v.size - lfanew < sizeof(IMAGE_NT_HEADERS64)) return std::unexpected(invalid_image(4));

    v.nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(v.bytes + lfanew);
    if (v.nt->Signature != IMAGE_NT_SIGNATURE) return std::unexpected(invalid_image(5));

    if (v.nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
        v.nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
        return std::unexpected(make_error(Error::architecture_mismatch, Stage::validate));
    if (!(v.nt->FileHeader.Characteristics & IMAGE_FILE_DLL)) return std::unexpected(invalid_image(6));

    const IMAGE_OPTIONAL_HEADER64& opt = v.nt->OptionalHeader;
    if (opt.SizeOfImage == 0 || opt.SizeOfHeaders > v.size || opt.SizeOfHeaders > opt.SizeOfImage)
        return std::unexpected(invalid_image(7));
    if (opt.SectionAlignment == 0 || (opt.SectionAlignment & (opt.SectionAlignment - 1)))
        return std::unexpected(invalid_image(8));

    const std::uint64_t sec_off = lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) +
                                  v.nt->FileHeader.SizeOfOptionalHeader;
    const std::uint64_t count = v.nt->FileHeader.NumberOfSections;
    if (sec_off + count * sizeof(IMAGE_SECTION_HEADER) > v.size) return std::unexpected(invalid_image(9));
    v.sections = reinterpret_cast<const IMAGE_SECTION_HEADER*>(v.bytes + sec_off);
    v.section_count = static_cast<std::uint32_t>(count);

    for (std::uint32_t i = 0; i < v.section_count; ++i) {
        const IMAGE_SECTION_HEADER& s = v.sections[i];
        const std::uint64_t mem = std::max<std::uint64_t>(s.Misc.VirtualSize, s.SizeOfRawData);
        if (static_cast<std::uint64_t>(s.VirtualAddress) + mem > opt.SizeOfImage)
            return std::unexpected(invalid_image(10 + i));
        if (s.SizeOfRawData && static_cast<std::uint64_t>(s.PointerToRawData) + s.SizeOfRawData > v.size)
            return std::unexpected(invalid_image(10 + i));
    }

    const IMAGE_DATA_DIRECTORY& reloc = opt.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    if (reloc.Size && static_cast<std::uint64_t>(reloc.VirtualAddress) + reloc.Size > opt.SizeOfImage)
        return std::unexpected(invalid_image(20));

    return v;
}

// Returns the bootstrap's failure as an Error_info.
Error_info bootstrap_failure(std::uint32_t stage, std::uint32_t detail) {
    switch (static_cast<Stage>(stage)) {
    case Stage::imports: return make_error(Error::import_failed, Stage::imports, 0, detail);
    case Stage::protect: return make_error(Error::protect_failed, Stage::protect, 0, detail);
    case Stage::seh:     return make_error(Error::seh_failed, Stage::seh, 0, detail);
    case Stage::entry:   return make_error(Error::entry_failed, Stage::entry, 0, detail);
    case Stage::relocate:return make_error(Error::unsupported, Stage::relocate, 0, detail);
    case Stage::tls:     return make_error(Error::entry_failed, Stage::tls, 0, detail);
    default:             return make_error(Error::internal, static_cast<Stage>(stage), 0, detail);
    }
}

Result<Module_handle> map_image(HANDLE proc, std::span<const std::byte> img, const Manual_map_options& opt) {
    auto pe = validate_pe(img);
    if (!pe) return std::unexpected(pe.error());

    const IMAGE_OPTIONAL_HEADER64& oh = pe->nt->OptionalHeader;
    const bool has_relocs = oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size != 0;

    // Protections need page-granular sections. With smaller alignment, sections can share a page.
    const bool adjust = opt.adjust_protections && oh.SectionAlignment >= 0x1000;
    const std::uint32_t initial_prot = adjust ? PAGE_READWRITE : PAGE_EXECUTE_READWRITE;

    // Try the preferred base first. This avoids relocations, and a fixed base is
    // the only way the image can be placed where its code expects.
    std::uint8_t* remote = nullptr;
    if (opt.prefer_image_base) {
        remote = static_cast<std::uint8_t*>(VirtualAllocEx(
            proc, reinterpret_cast<void*>(oh.ImageBase), oh.SizeOfImage, MEM_COMMIT | MEM_RESERVE, initial_prot));
    }
    if (!remote) {
        if (!has_relocs)
            return std::unexpected(make_error(Error::relocation_required, Stage::allocate));
        remote = static_cast<std::uint8_t*>(
            VirtualAllocEx(proc, nullptr, oh.SizeOfImage, MEM_COMMIT | MEM_RESERVE, initial_prot));
        if (!remote) return std::unexpected(make_error(Error::alloc_failed, Stage::allocate, last_error()));
    }

    auto release_image = [&] { VirtualFreeEx(proc, remote, 0, MEM_RELEASE); };

    if (!WriteProcessMemory(proc, remote, pe->bytes, oh.SizeOfHeaders, nullptr)) {
        auto err = last_error();
        release_image();
        return std::unexpected(make_error(Error::write_failed, Stage::write, err));
    }

    for (std::uint32_t i = 0; i < pe->section_count; ++i) {
        const IMAGE_SECTION_HEADER& s = pe->sections[i];
        if (!s.SizeOfRawData) continue;
        // Copy at most VirtualSize bytes when given; the rest of the section stays zeroed.
        const std::uint32_t n = s.Misc.VirtualSize ? std::min(s.SizeOfRawData, s.Misc.VirtualSize)
                                                   : s.SizeOfRawData;
        if (!WriteProcessMemory(proc, remote + s.VirtualAddress, pe->bytes + s.PointerToRawData, n, nullptr)) {
            auto err = last_error();
            release_image();
            return std::unexpected(make_error(Error::write_failed, Stage::write, err, i));
        }
    }

    // Bootstrap code and parameter block.
    const std::size_t bootstrap_len = bootstrap_size();
    if (!bootstrap_len) {
        release_image();
        return std::unexpected(make_error(Error::internal, Stage::allocate));
    }
    std::vector<std::uint8_t> bootstrap_code(bootstrap_len);
    std::memcpy(bootstrap_code.data(), reinterpret_cast<const void*>(&manual_map_bootstrap), bootstrap_len);

    auto* bootstrap_mem = static_cast<std::uint8_t*>(
        VirtualAllocEx(proc, nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!bootstrap_mem) {
        auto err = last_error();
        release_image();
        return std::unexpected(make_error(Error::alloc_failed, Stage::allocate, err));
    }
    auto* data_mem = static_cast<Bootstrap_params*>(
        VirtualAllocEx(proc, nullptr, sizeof(Bootstrap_params), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!data_mem) {
        auto err = last_error();
        VirtualFreeEx(proc, bootstrap_mem, 0, MEM_RELEASE);
        release_image();
        return std::unexpected(make_error(Error::alloc_failed, Stage::allocate, err));
    }

    auto release_bootstrap = [&] {
        VirtualFreeEx(proc, bootstrap_mem, 0, MEM_RELEASE);
        VirtualFreeEx(proc, data_mem, 0, MEM_RELEASE);
    };

    if (!WriteProcessMemory(proc, bootstrap_mem, bootstrap_code.data(), bootstrap_code.size(), nullptr)) {
        auto err = last_error();
        release_bootstrap();
        release_image();
        return std::unexpected(make_error(Error::write_failed, Stage::write, err));
    }

    // kernel32/ntdll are mapped at the same address in every process of one bitness per boot.
    HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    Bootstrap_params params{};
    params.load_library_a = reinterpret_cast<std::uint64_t>(GetProcAddress(k32, "LoadLibraryA"));
    params.get_proc_address = reinterpret_cast<std::uint64_t>(GetProcAddress(k32, "GetProcAddress"));
    params.virtual_protect = reinterpret_cast<std::uint64_t>(GetProcAddress(k32, "VirtualProtect"));
    params.rtl_add_function_table = reinterpret_cast<std::uint64_t>(GetProcAddress(ntdll, "RtlAddFunctionTable"));
    params.rtl_delete_function_table = reinterpret_cast<std::uint64_t>(GetProcAddress(ntdll, "RtlDeleteFunctionTable"));
    params.image_base = reinterpret_cast<std::uint64_t>(remote);
    params.reserved = opt.reserved;
    params.reason = opt.dll_reason;
    if (adjust) params.flags |= kFlagAdjustProtections;
    if (opt.enable_seh) params.flags |= kFlagEnableSeh;
    if (opt.enable_tls_callbacks) params.flags |= kFlagTlsCallbacks;
    if (opt.call_entry) params.flags |= kFlagCallEntry;

    if (!params.load_library_a || !params.get_proc_address || !params.virtual_protect ||
        (opt.enable_seh && (!params.rtl_add_function_table || !params.rtl_delete_function_table))) {
        release_bootstrap();
        release_image();
        return std::unexpected(make_error(Error::internal, Stage::resolve));
    }

    if (!WriteProcessMemory(proc, data_mem, &params, sizeof(params), nullptr)) {
        auto err = last_error();
        release_bootstrap();
        release_image();
        return std::unexpected(make_error(Error::write_failed, Stage::write, err));
    }

    HANDLE thread = CreateRemoteThread(proc, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(bootstrap_mem),
                                       data_mem, 0, nullptr);
    if (!thread) {
        auto err = last_error();
        release_bootstrap();
        release_image();
        return std::unexpected(make_error(Error::remote_thread_failed, Stage::remote, err));
    }

    const DWORD wait = WaitForSingleObject(thread, opt.timeout_ms);
    CloseHandle(thread);
    if (wait == WAIT_TIMEOUT) {
        // The bootstrap may still be running, so memory is not freed here.
        return std::unexpected(make_error(Error::timeout, Stage::remote));
    }

    Bootstrap_params out{};
    if (!ReadProcessMemory(proc, data_mem, &out, sizeof(out), nullptr)) {
        auto err = last_error();
        release_bootstrap();
        release_image();
        return std::unexpected(make_error(Error::remote_thread_failed, Stage::remote, err));
    }
    release_bootstrap();

    if (out.result != 1) {
        release_image();
        return std::unexpected(bootstrap_failure(out.fail_stage, out.fail_detail));
    }

    return Module_handle{reinterpret_cast<std::uintptr_t>(remote), oh.SizeOfImage};
}

class Manual_map_provider final : public Provider {
public:
    explicit Manual_map_provider(Manual_map_options o) : opt_(o) {}

    [[nodiscard]] Method method() const noexcept override { return Method::manual_map; }

    [[nodiscard]] Result<Module_handle> inject(const Target& target, const Module_source& src) override {
        std::vector<std::byte> owned;
        std::span<const std::byte> image;

        if (src.kind == Module_source::Kind::file) {
            auto data = read_file(src.path);
            if (!data) return std::unexpected(data.error());
            owned = std::move(*data);
            image = owned;
        } else {
            image = src.bytes;
        }
        if (image.empty()) return std::unexpected(make_error(Error::invalid_argument, Stage::validate));

        auto pid = resolve_pid(target);
        if (!pid) return std::unexpected(pid.error());

        auto proc = open_target(*pid);
        if (!proc) return std::unexpected(proc.error());

        return map_image(static_cast<HANDLE>(proc->get()), image, opt_);
    }

private:
    Manual_map_options opt_;
};

} // namespace

std::unique_ptr<Provider> make_manual_map_provider(const Manual_map_options& o) {
    return std::make_unique<Manual_map_provider>(o);
}

} // namespace inject::detail
