#pragma once
// PakUnmountFix.log, in the mod's folder. DEBUG_LOG lines are only written by a build with PAK_UNMOUNT_FIX_DEBUG.

#ifndef PAK_UNMOUNT_FIX_DEBUG
#define PAK_UNMOUNT_FIX_DEBUG 0
#endif

void log_message(const char *format, ...);

#if PAK_UNMOUNT_FIX_DEBUG
#define DEBUG_LOG(...) log_message(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif
