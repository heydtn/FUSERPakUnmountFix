#include "hooks.hpp"

#include <string>

#include "log.hpp"
#include "mod.hpp"

uint8_t *find_function(const char *name, const char *pattern, uint8_t *start, size_t size) {
    uint8_t *found = g_loader->find_pattern(g_this_mod, name, pattern, start, size);
    if (found) {
        DEBUG_LOG(
            std::string(name) + " found at exe+0x" + hex_text(static_cast<std::uintptr_t>(found - g_loader->game->base))
        );
    } else {
        DEBUG_LOG(std::string(name) + " not found");
    }
    return found;
}

std::string other_owners(const void *at, size_t size) {
    if (!FNL_API_HAS(g_loader, changed)) {
        return {};
    }
    char owners[256] = {};
    const size_t count = g_loader->changed(g_this_mod, at, size, FNL_CHANGED_BY_OTHERS, owners, sizeof(owners));
    return count != 0 && count != FNL_CHANGED_ERROR ? std::string(owners) : std::string();
}

bool setup_hooks(const FnlHook *hooks, size_t count) {
    return g_loader->hook(g_this_mod, hooks, count) != 0;
}

bool redirect_import_pointer(const char *dll, const char *function, void *hook, void **original) {
    return g_loader->redirect_import(g_this_mod, dll, function, hook, original) != 0;
}

bool write_pointer(void **slot, void *value) {
    return g_loader->patch(g_this_mod, slot, &value, sizeof(value)) != 0;
}
