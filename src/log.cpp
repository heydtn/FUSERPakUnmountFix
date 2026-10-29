#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "log.hpp"

#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <share.h>
#include <string>

namespace {

// The log file, opened at the first message over the previous launch's
std::FILE *g_log = nullptr;

// Opens g_log once, whichever thread logs first
std::once_flag g_log_opened;

std::wstring log_path() {
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&log_path), &self);
    std::wstring path(MAX_PATH, L'\0');
    for (;;) {
        const DWORD length = GetModuleFileNameW(self, path.data(), static_cast<DWORD>(path.size()));
        if (length == 0) {
            return L"PakUnmountFix.log";
        }
        if (length < path.size()) {
            path.resize(length);
            break;
        }
        path.resize(path.size() * 2);
    }
    return path.substr(0, path.find_last_of(L'\\') + 1) + L"PakUnmountFix.log";
}

}  // namespace

void start_log() {
    std::call_once(g_log_opened, [] { g_log = _wfsopen(log_path().c_str(), L"w", _SH_DENYNO); });
}

void log_error(const char *format, ...) {
    start_log();
    if (!g_log) {
        return;
    }
    va_list arguments;
    va_start(arguments, format);
    std::vfprintf(g_log, format, arguments);
    va_end(arguments);
    std::fflush(g_log);
}
