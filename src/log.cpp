#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "log.hpp"

#include <string>

#include "mod.hpp"

namespace {

std::wstring wide_text(const std::string &text) {
    const int length = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    std::wstring wide(static_cast<size_t>(length), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), wide.data(), length);
    return wide;
}

void log_line(int level, const std::string &line) {
    if (g_loader) {
        g_loader->log(g_this_mod, level, wide_text(line).c_str());
    }
}

}  // namespace

void log_error(const std::string &line) {
    log_line(FNL_LOG_ERROR, line);
}

void log_warning(const std::string &line) {
    log_line(FNL_LOG_WARNING, line);
}

void log_verbose(const std::string &line) {
    log_line(FNL_LOG_VERBOSE, line);
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
