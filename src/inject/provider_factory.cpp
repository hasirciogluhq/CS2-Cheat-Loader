#include "inject/injector.hpp"
#include "inject/win_util.hpp"

namespace inject {

std::string_view to_string(Error e) noexcept {
    switch (e) {
    case Error::invalid_argument:       return "invalid argument";
    case Error::invalid_options:        return "invalid options";
    case Error::process_not_found:      return "process not found";
    case Error::process_open_failed:    return "failed to open process";
    case Error::architecture_mismatch:  return "architecture mismatch (target must be 64-bit)";
    case Error::file_not_found:         return "file not found";
    case Error::file_read_failed:       return "failed to read file";
    case Error::invalid_image:          return "invalid PE image";
    case Error::unsupported:            return "unsupported by this method";
    case Error::relocation_required:    return "image needs relocation but has no base relocations";
    case Error::alloc_failed:           return "remote allocation failed";
    case Error::write_failed:           return "remote write failed";
    case Error::protect_failed:         return "section protection failed";
    case Error::remote_thread_failed:   return "remote thread failed";
    case Error::timeout:                return "remote thread timed out";
    case Error::module_not_found:       return "module not found in target after load";
    case Error::import_failed:          return "import resolution failed";
    case Error::seh_failed:             return "exception table registration failed";
    case Error::entry_failed:           return "entry point / DllMain failed";
    case Error::internal:               return "internal error";
    }
    return "unknown error";
}

std::string_view to_string(Stage s) noexcept {
    switch (s) {
    case Stage::none:     return "none";
    case Stage::validate: return "validate";
    case Stage::resolve:  return "resolve process";
    case Stage::open:     return "open process";
    case Stage::allocate: return "allocate";
    case Stage::write:    return "write";
    case Stage::remote:   return "remote thread";
    case Stage::verify:   return "verify";
    case Stage::relocate: return "relocate";
    case Stage::imports:  return "imports";
    case Stage::protect:  return "protect";
    case Stage::seh:      return "seh";
    case Stage::tls:      return "tls";
    case Stage::entry:    return "entry";
    }
    return "unknown stage";
}

Method method_of(const Options& o) noexcept {
    return std::holds_alternative<Load_library_options>(o) ? Method::load_library : Method::manual_map;
}

Result<void> validate(const Options& o) {
    if (const auto* ll = std::get_if<Load_library_options>(&o)) {
        if (ll->timeout_ms == 0) return std::unexpected(Error_info{Error::invalid_options, Stage::validate});
        return {};
    }
    const auto& mm = std::get<Manual_map_options>(o);
    if (mm.timeout_ms == 0) return std::unexpected(Error_info{Error::invalid_options, Stage::validate});
    // DllMain reasons are 0..3. Anything else is a caller bug.
    if (mm.dll_reason > 3) return std::unexpected(Error_info{Error::invalid_options, Stage::validate, 0, mm.dll_reason});
    return {};
}

Result<std::unique_ptr<Provider>> make_provider(const Options& opts) {
    if (auto v = validate(opts); !v) return std::unexpected(v.error());

    if (const auto* ll = std::get_if<Load_library_options>(&opts))
        return detail::make_load_library_provider(*ll);
    return detail::make_manual_map_provider(std::get<Manual_map_options>(opts));
}

Result<Module_handle> inject(const Target& target, const Module_source& src, const Options& opts) {
    auto p = make_provider(opts);
    if (!p) return std::unexpected(p.error());
    return (*p)->inject(target, src);
}

} // namespace inject
