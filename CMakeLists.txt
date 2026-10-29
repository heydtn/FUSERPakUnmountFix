cmake_minimum_required(VERSION 3.20)
project(PakUnmountFix LANGUAGES C CXX ASM_MASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PAK_UNMOUNT_FIX_DEBUG "Also log every reader, stub and hooked handle class" OFF)

include(FetchContent)
FetchContent_Declare(funchook
    GIT_REPOSITORY https://github.com/kubo/funchook.git
    GIT_TAG v1.1.3
    GIT_SHALLOW TRUE)
set(FUNCHOOK_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(FUNCHOOK_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FUNCHOOK_INSTALL OFF CACHE BOOL "" FORCE)
set(FUNCHOOK_DISASM distorm CACHE STRING "" FORCE)
FetchContent_MakeAvailable(funchook)

add_library(PakUnmountFix SHARED
    src/mod.cpp
    src/pak_unmount_fix.cpp
    src/hooks.cpp
    src/log.cpp
    src/pak_return_thunk.asm
)
set_source_files_properties(src/pak_return_thunk.asm PROPERTIES LANGUAGE ASM_MASM)
target_include_directories(PakUnmountFix PRIVATE include src)
target_link_libraries(PakUnmountFix PRIVATE funchook-static)
target_compile_definitions(PakUnmountFix PRIVATE UNICODE _UNICODE
    PAK_UNMOUNT_FIX_DEBUG=$<BOOL:${PAK_UNMOUNT_FIX_DEBUG}>)
target_compile_options(PakUnmountFix PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:/W4;/wd4324;/utf-8;/EHsc;/Zi>")
target_link_options(PakUnmountFix PRIVATE /DEBUG /OPT:REF /OPT:ICF)
