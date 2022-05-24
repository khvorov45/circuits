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

typedef struct PoolMarker {
	struct PoolChunk* chunk;
	b32 freeTillNext;
	struct PoolMarker* next;
	struct PoolMarker* prev;
} PoolMarker;

typedef struct PoolChunk {
	i32 size;
	PoolMarker* firstMarker;
	struct PoolChunk* next;
	struct PoolChunk* prev;
} PoolChunk;

typedef struct MemoryPool {
	i32 minChunkSize;
	PoolChunk* firstChunk;
	Allocator chunkAllocator;
} MemoryPool;

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

function AllocatorError
mempoolInit(MemoryPool* pool, i32 minChunkSize, Allocator chunkAllocator) {
	zeroPtr(pool);
	AllocatorProcResult firstChunkAllocResult =
		chunkAllocator.proc(chunkAllocator.data, AllocatorMode_Alloc, minChunkSize, sizeof(void*), 0);

	if (firstChunkAllocResult.err == AllocatorError_None) {
		pool->minChunkSize = minChunkSize;
		pool->firstChunk = (PoolChunk*)(firstChunkAllocResult.ptr);
		pool->chunkAllocator = chunkAllocator;

		zeroPtr(pool->firstChunk);
		pool->firstChunk->size = minChunkSize;

		pool->firstChunk->firstMarker = (PoolMarker*)((u8*)firstChunkAllocResult.ptr + sizeof(PoolChunk));
		zeroPtr(pool->firstChunk->firstMarker);
		pool->firstChunk->firstMarker->freeTillNext = true;
		pool->firstChunk->firstMarker->chunk = pool->firstChunk;
	}

	return firstChunkAllocResult.err;
}

function AllocatorProcResult
mempoolAllocatorProc(void* data, AllocatorMode mode, i32 size, i32 align, void* oldptr) {
	AllocatorProcResult result = {0};

	MemoryPool* pool = (MemoryPool*)data;

	switch (mode) {
	case AllocatorMode_Alloc: {
		b32 found = false;
		for (PoolChunk* chunk = pool->firstChunk; chunk != 0 && !found; chunk = chunk->next) {
			for (PoolMarker* marker = chunk->firstMarker; marker != 0 && !found; marker = marker->next) {
				if (marker->freeTillNext) {

					usize nextAddress;
					if (marker->next == 0) {
						nextAddress = (usize)((u8*)(chunk) + (usize)(chunk->size));
					} else {
						nextAddress = (usize)marker->next;
					}

					usize freeStart = (usize)((u8*)(marker) + sizeof(PoolMarker));
					usize freeBytes = nextAddress - (usize)freeStart;

					void* testData = (void*)freeStart;
					i32 sizeAligned = size;
					alignPtr(&testData, align, &sizeAligned);
					if ((usize)sizeAligned <= freeBytes) {
						result.ptr = testData;
						found = true;
						marker->freeTillNext = false;

						PoolMarker markerCopy = *marker;
						marker = (PoolMarker*)((u8*)result.ptr - sizeof(PoolMarker));
						*marker = markerCopy;
						if (marker->prev != 0) {
							marker->prev->next = marker;
						} else {
							chunk->firstMarker = marker;
						}
						if (marker->next != 0) {
							marker->next->prev = marker;
						}

						if (freeBytes - (usize)sizeAligned >= sizeof(PoolMarker) + 1024) {
							PoolMarker* newMarker = (PoolMarker*)((u8*)result.ptr + size);
							newMarker->next = marker->next;
							newMarker->prev = marker;
							newMarker->freeTillNext = true;

							marker->next = newMarker;
							if (newMarker->next) {
								newMarker->next->prev = newMarker;
							}
						}
					}
				}
			}
		}

		if (!found) {
			assert(!"allocate another chunk");
		}
	} break;

	case AllocatorMode_Free: {
		PoolMarker* marker = (PoolMarker*)((u8*)oldptr - sizeof(PoolMarker));
		assert(!marker->freeTillNext);
		marker->freeTillNext = true;

		for (PoolMarker* nextMarker = marker->next;;) {
			if (nextMarker == 0) {
				marker->next = 0;
				break;
			} else {
				if (nextMarker->freeTillNext) {
					nextMarker = nextMarker->next;
				} else {
					marker->next = nextMarker;
					marker->next->prev = marker;
					break;
				}
			}
		}

		if (marker->prev == 0) {
			PoolMarker markerCopy = *marker;
			PoolChunk* chunk = markerCopy.chunk;
			chunk->firstMarker = (PoolMarker*)((u8*)chunk + sizeof(PoolChunk));
			*(chunk->firstMarker) = markerCopy;
			if (chunk->firstMarker->next != 0) {
				chunk->firstMarker->next->prev = chunk->firstMarker;
			}
		} else {
			for (PoolMarker* prevMarker = marker->prev; prevMarker != 0 && prevMarker->freeTillNext;) {
				prevMarker->next = marker->next;
				if (prevMarker->next != 0) {
					prevMarker->next->prev = prevMarker;
				}
				prevMarker = prevMarker->prev;
			}
		}
	} break;

	case AllocatorMode_Resize: {
		assert(!"pool resize");
	} break;
	}

	return result;
}

function Allocator
mempoolAllocatorCreate(MemoryPool* pool) {
	Allocator result = {pool, mempoolAllocatorProc};
	return result;
}
