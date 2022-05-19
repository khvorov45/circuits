typedef enum AllocatorError {
	AllocatorError_None,
	AllocatorError_OutOfMemory
} AllocatorError;

typedef enum AllocatorMode {
	AllocatorMode_Alloc,
	AllocatorMode_Free,
	AllocatorMode_Resize,
} AllocatorMode;

typedef struct AllocatorProcResult {
	void* ptr;
	AllocatorError err;
} AllocatorProcResult;

typedef AllocatorProcResult (*AllocatorProc)(void* data, AllocatorMode mode, i32 size, i32 align, void* oldptr);

typedef struct Allocator {
	void* data;
	AllocatorProc proc;
} Allocator;

typedef struct VirtualArena {
	void* base;
	i32 reserved;
	i32 committed;
	i32 used;
} VirtualArena;

#if CIRCUITS_WINDOWS
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
#endif

function void
alignPtr(void** ptr, i32 align, i32* size) {
	assert(isPowerOf2(align));
	void* ptrOg = *ptr;
	i32 offBy = ((usize)ptrOg) & (align - 1);
	i32 moveBy = 0;
	if (offBy > 0) {
		moveBy = align - offBy;
	}
	void* ptrAligned = (u8*)ptrOg + moveBy;
	*ptr = ptrAligned;
	*size = *size + moveBy;
}

function void*
alloc(i32 size, i32 align, Allocator allocator) {
	AllocatorProcResult allocResult = allocator.proc(allocator.data, AllocatorMode_Alloc, size, align, 0);
	assert(allocResult.err == AllocatorError_None);
	return allocResult.ptr;
}

function void
varenaInit(VirtualArena* varena, i32 reserve, i32 initCommit) {
	assert(initCommit <= reserve);
	zeroPtr(varena);
	vmemReserve(reserve, &varena->base, &varena->reserved);
	vmemCommit(varena->base, initCommit, &varena->committed);
}

function AllocatorProcResult
varenaAllocatorProc(void* data, AllocatorMode mode, i32 size, i32 align, void* oldptr) {
	AllocatorProcResult result = {0};
	VirtualArena* varena = (VirtualArena*)data;
	assert(varena->committed >= varena->used);
	assert(varena->reserved >= varena->committed);

	switch (mode) {
	case AllocatorMode_Alloc: {

		void* baseAligned = (u8*)varena->base + varena->used;
		i32 sizeAligned = size;
		alignPtr(&baseAligned, align, &sizeAligned);

		i32 committedAndFree = varena->committed - varena->used;
		i32 reservedAndFree = varena->reserved - varena->used;

		if (committedAndFree >= sizeAligned) {
			result.ptr = baseAligned;
			varena->used += sizeAligned;
		} else if (reservedAndFree >= sizeAligned) {
			vmemCommit(varena->base, varena->used + sizeAligned, &varena->committed);
			result.ptr = baseAligned;
			varena->used += sizeAligned;
		} else {
			result.err = AllocatorError_OutOfMemory;
		}
	} break;

	case AllocatorMode_Free: {
		assert(!"VirtualArena called with Free");
	} break;

	case AllocatorMode_Resize: {
		assert(!"VirtualArena called with Resize");
	} break;
	}

	return result;
}

function Allocator
varenaAllocatorCreate(VirtualArena* varena) {
	Allocator result = {varena, varenaAllocatorProc};
	return result;
}
