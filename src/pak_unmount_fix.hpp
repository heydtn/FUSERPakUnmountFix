#pragma once

// Fixes the crash when an unmounted pak is still in use (scrolling the song list); see pak_unmount_fix.cpp

void ScanForPakUnmountFix();
void ApplyPakUnmountFix();
