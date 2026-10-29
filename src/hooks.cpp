#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "hooks.hpp"

#include <cstdlib>
#include <cstring>
#include <vector>

#include <funchook.h>

#include "log.hpp"
#include "pe_image.hpp"

namespace {

constexpr int kAnyByte = -1;

std::vector<int> parse_pattern(const char *pattern) {
    std::vector<int> bytes;
    for (size_t i = 0; pattern[i] && pattern[i + 1]; i += 2) {
        if (pattern[i] == '?') {
            bytes.push_back(kAnyByte);
            continue;
        }
        const char digits[3] = {pattern[i], pattern[i + 1], '\0'};
        bytes.push_back(static_cast<int>(std::strtol(digits, nullptr, 16)));
    }
    return bytes;
}

bool matches_at(const uint8_t *at, const std::vector<int> &bytes) {
    for (size_t i = 0; i < bytes.size(); i++) {
        if (bytes[i] != kAnyByte && at[i] != bytes[i]) {
            return false;
        }
    }
    return true;
}

uint8_t *exe_base() {
    return reinterpret_cast<uint8_t *>(GetModuleHandleW(nullptr));
}

bool trampoline_within_jump_range(const void *function, const void *trampoline) {
    const int64_t distance = static_cast<int64_t>(reinterpret_cast<uintptr_t>(trampoline)) -
                             static_cast<int64_t>(reinterpret_cast<uintptr_t>(function) + 5);
    return distance > -0x7FFF0000LL && distance < 0x7FFF0000LL;
}

}  // namespace

uint8_t *find_function([[maybe_unused]] const char *name, const char *pattern, uint8_t *start, size_t size) {
    const std::vector<int> bytes = parse_pattern(pattern);
    if (bytes.empty() || bytes.size() > size) {
        return nullptr;
    }
    const size_t last = size - bytes.size();
    for (size_t offset = 0; offset <= last; offset++) {
        if (bytes[0] != kAnyByte) {
            const void *next = std::memchr(start + offset, bytes[0], last - offset + 1);
            if (!next) {
                break;
            }
            offset = static_cast<const uint8_t *>(next) - start;
        }
        if (matches_at(start + offset, bytes)) {
            DEBUG_LOG("%s found at exe+0x%llX\n", name,
                      static_cast<unsigned long long>(start + offset - exe_base()));
            return start + offset;
        }
    }
    DEBUG_LOG("%s not found\n", name);
    return nullptr;
}

uint8_t *find_function(const char *name, const char *pattern) {
    uint8_t *exe = exe_base();
    return find_function(name, pattern, exe, nt_headers(exe)->OptionalHeader.SizeOfImage);
}

bool setup_hooks(const HookRequest *hooks, size_t count) {
    funchook_t *handle = funchook_create();
    if (!handle) {
        log_message("funchook_create failed - not hooked\n");
        return false;
    }
    std::vector<void *> originals(count);
    for (size_t i = 0; i < count; i++) {
        originals[i] = *hooks[i].target;
        if (funchook_prepare(handle, hooks[i].target, hooks[i].hook) != FUNCHOOK_ERROR_SUCCESS) {
            log_message("funchook_prepare failed for %p: %s - not hooked\n", originals[i],
                        funchook_error_message(handle));
        } else if (!trampoline_within_jump_range(originals[i], *hooks[i].target)) {
            log_message("funchook's trampoline %p is out of jump range of %p - not hooked\n", *hooks[i].target,
                        originals[i]);
        } else {
            continue;
        }
        funchook_destroy(handle);
        for (size_t j = 0; j <= i; j++) {
            *hooks[j].target = originals[j];
        }
        return false;
    }
    if (funchook_install(handle, 0) != FUNCHOOK_ERROR_SUCCESS) {
        log_message("funchook_install failed: %s - not hooked\n", funchook_error_message(handle));
        return false;
    }
    return true;
}
