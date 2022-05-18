#include <stdint.h>

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

//
// SECTION CRT
//

int _fltused = 0x9875;
