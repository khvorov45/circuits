function void
vmemReserve(i32 size, void** ptr, i32* sizeActual) {
	*ptr = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
	*sizeActual = size;
}

function void
vmemCommit(void* ptr, i32 size, i32* sizeActual) {
	VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
	*sizeActual = size;
}

#include "mem.c"
