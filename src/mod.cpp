// PakUnmountFix's FUSERModLoader entry points: find the pak code before any mod patches the game, then hook it.
#include "fuser_mod_loader.h"
#include "log.hpp"
#include "pak_unmount_fix.hpp"

FUSER_MOD_EXPORT void FuserModScan(void) {
    start_log();
    ScanForPakUnmountFix();
}

FUSER_MOD_EXPORT void FuserModApply(void) {
    ApplyPakUnmountFix();
}
