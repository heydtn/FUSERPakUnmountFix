# PakUnmountFix

A FUSER mod that fixes the game crashing when you scroll the song list quickly with custom songs installed. It needs
[FUSERNativeLoader](https://github.com/unreality/FUSERNativeLoader).

## Installing

Once [FUSERNativeLoader is installed](https://github.com/unreality/FUSERNativeLoader#using-the-mod), unzip the
release's `PakUnmountFix.zip` into `Fuser\Binaries\Win64\mods`. The file structure should be:

```
Fuser\Binaries\Win64\mods\PakUnmountFix\
  PakUnmountFix.dll
  mod.ini
```

If the fix can't be applied, `Fuser\Binaries\Win64\FuserNative.log` says why, on lines tagged
`[PakUnmountFix]`.

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

## Building

Needs Visual Studio 2022 with the **Desktop development with C++** workload. From a Developer PowerShell:

```
cmake -S . -B build -A x64
cmake --build build --config Release
```

This builds `build\Release\PakUnmountFix.dll`.

## Releases

Releases are published by hand: on GitHub, open Actions, pick the workflow and use **Run workflow** on
`main`. Each release is named after the commit's time in UTC and its hash, as
`YYYY.MM.DD.HHMMSS-<commit>`, with `PakUnmountFix.zip` holding `PakUnmountFix.dll` and `mod.ini` in a
`PakUnmountFix` folder.

## Licence

MIT; see [LICENSE](LICENSE). `include/fnl_mod.h` is FUSERNativeLoader's SDK header, copied unchanged from
[its `sdk` folder](https://github.com/unreality/FUSERNativeLoader/tree/70ff046fd3ff10df86189912b417f667780044a5/sdk)
(MIT).

## AI Usage Disclosure

This mod's code, behavior and methods were reviewed by a human who has a career in programming and knows binary
analysis. The binary analysis was AI-assisted, and the code was AI-generated, then reviewed, adjusted and modified by
that human.
