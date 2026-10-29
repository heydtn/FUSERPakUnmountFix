#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "log.hpp"

#include <cstdio>
#include <memory>
#include <mutex>
#include <share.h>
#include <string>

namespace {

struct FileCloser {
    void operator()(std::FILE *file) const { std::fclose(file); }
};

// The log file, opened at the first message over the previous launch's
std::unique_ptr<std::FILE, FileCloser> g_log;

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
    std::call_once(g_log_opened, [] { g_log.reset(_wfsopen(log_path().c_str(), L"w", _SH_DENYNO)); });
}

void log_error(const std::string &line) {
    start_log();
    if (!g_log) {
        return;
    }
    std::fputs(line.c_str(), g_log.get());
    std::fputc('\n', g_log.get());
    std::fflush(g_log.get());
}

std::string hex_text(std::uintptr_t value) {
    constexpr char kDigits[] = "0123456789ABCDEF";
    std::string text;
    do {
        text.insert(text.begin(), kDigits[value % 16]);
        value /= 16;
    } while (value != 0);
    return text;
}

std::string pointer_text(const void *pointer) {
    return "0x" + hex_text(reinterpret_cast<std::uintptr_t>(pointer));
}
