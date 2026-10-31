// PakUnmountFix's FUSERNativeLoader entry point, run in the early stage before any pak is mounted.
#include "mod.hpp"
#include "log.hpp"
#include "pak_unmount_fix.hpp"

// The loader's API table
const FnlApi *g_loader = nullptr;

// The loader's record of this mod, passed back on every API call
FnlMod *g_this_mod = nullptr;

extern "C" FNL_MOD_EXPORT int fnl_mod_init(const FnlApi *api, FnlMod *self) {
    if (!api || api->abi < FNL_MOD_ABI || !FNL_API_HAS(api, redirect_import)) {
        return FNL_FAILED;
    }
    g_loader = api;
    g_this_mod = self;
    if (!FNL_API_HAS(api, changed)) {
        log_warning("Detected an older FUSERNativeLoader. The fix may still work, but it can't check whether other "
                    "mods have changed the pak code it reads, so upgrading to a FUSERNativeLoader newer than 0.16.0 "
                    "is recommended.");
    }
    return ApplyPakUnmountFix();
}
