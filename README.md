# PakUnmountFix

A FUSER mod that fixes the game crashing when you scroll the song list quickly with custom songs installed. It needs
[FUSERModLoader](https://github.com/heydtn/FUSERModLoader).

## The crash

UE 4.24's `FPakPlatformFile` code copies the list of mounted paks while holding a lock, lets go of the lock, and
then uses the paks in its copy. Unmounting a pak deletes it while holding that same lock. So a pak can be deleted
while another thread is still reading it from its copy. Scrolling the song list mounts and unmounts song paks
quickly, which makes this happen often enough to crash.

## The fix

Unmount's delete is deferred until nothing can still be using the pak:

- Every thread that takes the pak lock is a reader until its function returns. The mod sees it take the lock
  through the game's `EnterCriticalSection` import, and points the function's return address at a small stub that
  marks the reader done.
- A pak unmounted while readers are running is freed once all of those readers have returned and the engine's own
  handle counts for it are zero. The delete runs on a thread-pool thread, and Unmount never waits.
- If anything isn't as expected (a byte pattern missing, a reader it can't track, or Windows enforcing shadow
  stacks), the mod logs why and the game deletes paks immediately, as it does without the mod.

## Installing

Needs FUSERModLoader's `winhttp.dll` in `Fuser\Binaries\Win64`. Copy `PakUnmountFix.dll` and `mod.ini` to:

```
Fuser\Binaries\Win64\mods\PakUnmountFix\PakUnmountFix.dll
Fuser\Binaries\Win64\mods\PakUnmountFix\mod.ini
```

`mod.ini` puts the mod in FUSERModLoader's `engine_fixes` stage, so it loads before other mods.

If the fix can't be applied, `PakUnmountFix.log` next to it says why.

## Building

Needs Visual Studio 2022 with the **Desktop development with C++** workload, and git on `PATH`, since CMake
downloads funchook while configuring. From a Developer PowerShell:

```
cmake -S . -B build -A x64
cmake --build build --config Release
```

This builds `build\Release\PakUnmountFix.dll`.

## Licence

MIT; see [LICENSE](LICENSE). Function hooking is done by [funchook](https://github.com/kubo/funchook) v1.1.3
(GPLv2 or later with a linking exception), which is built from source and linked in.
