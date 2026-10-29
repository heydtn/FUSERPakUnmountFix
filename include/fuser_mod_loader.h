#pragma once
// The optional entry points FUSERModLoader calls in a mod: every mod's FuserModScan, then every mod's FuserModApply,
// in load order. See FUSERModLoader's docs/WRITING_MODS.md.

#ifdef __cplusplus
#define FUSER_MOD_EXPORT extern "C" __declspec(dllexport)
#else
#define FUSER_MOD_EXPORT __declspec(dllexport)
#endif

typedef void (*FuserModScanFunction)(void);
typedef void (*FuserModApplyFunction)(void);
