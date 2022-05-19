#if CIRCUITS_WINDOWS
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdint.h>

#if CIRCUITS_WINDOWS
#define assert(cond) do { if (!(cond)) __debugbreak(); } while (0)
#endif

#define function static
#define true 1
#define false 0

#define BYTE 1
#define KILOBYTE 1024 * BYTE
#define MEGABYTE 1024 * KILOBYTE
#define GIGABYTE 1024 * MEGABYTE

#define zeroPtr(ptr) zeroPtr_((ptr), sizeof(*(ptr)))
#define isPowerOf2(x) (((x) & ((x) - 1)) == 0)
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define clamp(x, min, max) (((x) < (min)) ? (min) : ((x) > (max)) ? (max) : (x))

typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef intptr_t isize;
typedef uintptr_t usize;

typedef int32_t b32;

typedef float f32;
typedef double f64;

function void
zeroPtr_(void* ptrInit, usize size) {
	u8* ptr = ptrInit;
	while (size--) {*ptr++ = 0;}
}

#include "math.c"
#include "mem.c"
#include "window.c"
#include "renderer.c"

typedef struct Chip {
	i32 inputCount, outputCount;
} Chip;

function void 
drawChip(Renderer* renderer, Chip chip, v2i center) {
	v2i dim = {100, 100};
	Rect2i bodyRect = rectCenterDim(center, dim);
	v4 col = {1, 0, 0, 1};
	drawRect(renderer, bodyRect, col);

	v2i bodyRectBottomright = getRectBottomright(bodyRect);

	v4 pinCol = {1, 1, 0, 1};
	i32 pinGirth = 2;
	i32 pinLength = 50;

	i32 pinStep = bodyRect.dim.x / (chip.inputCount + 1);
	Rect2i curPinRect = {{bodyRect.topleft.x + pinStep, bodyRectBottomright.y}, {pinGirth, pinLength}};
	for (i32 inputIndex = 0; inputIndex < chip.inputCount; inputIndex += 1) {

		drawRect(renderer, curPinRect, pinCol);
		curPinRect.topleft.x += pinStep;
	}
}

function void
circuitsMain() {
	VirtualArena varena;
	varenaInit(&varena, 1 * GIGABYTE, 1 * MEGABYTE);
	Allocator varenaAllocator = varenaAllocatorCreate(&varena);

	AppWindow window_ = {0};
	AppWindow* window = &window_;
	initWindow(window, 1000, 1000);

	Renderer renderer_ = {0};
	Renderer* renderer = &renderer_;
	initRenderer(renderer, 7680, 4320, varenaAllocator);

	v4 clearCol = {0, 0, 0, 0};

	while (window->isRunning) {
		waitForInput(window);

		clearBuffers(renderer, window->dim.x, window->dim.y, clearCol);

		Chip tempChip = {2, 1};
		v2i tempChipCenter = {500, 500};
		drawChip(renderer, tempChip, tempChipCenter);

		displayPixels(window, renderer->pixels, renderer->dim.x, renderer->dim.y);
	}
}

//
// SECTION Entry points
//

#if CIRCUITS_WINDOWS
void
WinMainCRTStartup() {
	circuitsMain();
	ExitProcess(0);
}
#endif

//
// SECTION CRT
//

int _fltused = 0x9875;
