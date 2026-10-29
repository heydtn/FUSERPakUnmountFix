#pragma once
// PakUnmountFix.log, in the mod's folder. DEBUG_LOG lines are only written by debug builds.

#ifndef PAK_UNMOUNT_FIX_DEBUG
#define PAK_UNMOUNT_FIX_DEBUG 0
#endif

#if !defined(NDEBUG) || PAK_UNMOUNT_FIX_DEBUG
#define PAK_UNMOUNT_FIX_DEBUG_LOGGING 1
#else
#define PAK_UNMOUNT_FIX_DEBUG_LOGGING 0
#endif

void start_log();
void log_error(const char *format, ...);

#if PAK_UNMOUNT_FIX_DEBUG_LOGGING
#define DEBUG_LOG(...) log_error(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif
