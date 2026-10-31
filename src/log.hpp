#pragma once
// Lines in FUSERNativeLoader's FuserNative.log, prefixed with the mod's folder name. DEBUG_LOG lines are only written
// by debug builds.
#include <cstdint>
#include <string>

#ifndef PAK_UNMOUNT_FIX_DEBUG
#define PAK_UNMOUNT_FIX_DEBUG 0
#endif

#if !defined(NDEBUG) || PAK_UNMOUNT_FIX_DEBUG
#define PAK_UNMOUNT_FIX_DEBUG_LOGGING 1
#else
#define PAK_UNMOUNT_FIX_DEBUG_LOGGING 0
#endif

void log_error(const std::string &line);
void log_warning(const std::string &line);
void log_verbose(const std::string &line);
std::string hex_text(std::uintptr_t value);

#if PAK_UNMOUNT_FIX_DEBUG_LOGGING
#define DEBUG_LOG(line) log_verbose(line)
#else
#define DEBUG_LOG(line) static_cast<void>(0)
#endif
