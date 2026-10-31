#pragma once
// The game's code is searched and patched only through FUSERNativeLoader, so every mod's searches see it as shipped.
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

#include "fnl_mod.h"

uint8_t *find_function(const char *name, const char *pattern, uint8_t *start, size_t size);
std::string other_owners(const void *at, size_t size);

template <typename Function>
FnlHook hook_request(const char *name, Function &target, Function hook) {
    static_assert(std::is_pointer_v<Function> && std::is_function_v<std::remove_pointer_t<Function>>);
    return {name, reinterpret_cast<void **>(&target), reinterpret_cast<void *>(hook)};
}

bool setup_hooks(const FnlHook *hooks, size_t count);

bool redirect_import_pointer(const char *dll, const char *function, void *hook, void **original);
bool write_pointer(void **slot, void *value);

template <typename Function>
bool redirect_import(const char *dll, const char *function, Function hook, Function *original) {
    static_assert(std::is_pointer_v<Function> && std::is_function_v<std::remove_pointer_t<Function>>);
    return redirect_import_pointer(dll, function, reinterpret_cast<void *>(hook), reinterpret_cast<void **>(original));
}

template <typename Function>
bool redirect_slot(void **slot, Function hook, Function *original) {
    static_assert(std::is_pointer_v<Function> && std::is_function_v<std::remove_pointer_t<Function>>);
    *original = reinterpret_cast<Function>(*slot);
    return write_pointer(slot, reinterpret_cast<void *>(hook));
}
