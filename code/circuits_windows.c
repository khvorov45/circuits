#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "circuits.h"
#define assert(cond) do { if (!(cond)) __debugbreak(); } while (0)

#include "math.c"
#include "mem_windows.c"
#include "window_windows.c"
#include "renderer.c"
#include "circuits.c"

void
WinMainCRTStartup() {
	circuitsMain();
	ExitProcess(0);
}
