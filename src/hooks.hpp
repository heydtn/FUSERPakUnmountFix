#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>

uint8_t *find_function(const char *name, const char *pattern);
uint8_t *find_function(const char *name, const char *pattern, uint8_t *start, size_t size);

struct HookRequest {
    void **target;
    void *hook;
};

template <typename Function>
HookRequest hook_request(Function &target, Function hook) {
    static_assert(std::is_pointer_v<Function> && std::is_function_v<std::remove_pointer_t<Function>>);
    return {reinterpret_cast<void **>(&target), reinterpret_cast<void *>(hook)};
}

bool setup_hooks(const HookRequest *hooks, size_t count);
