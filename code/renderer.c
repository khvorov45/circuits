typedef struct Renderer {
	u32* pixels;
	v2i dim;
} Renderer;

function void
initRenderer(Renderer* renderer, i32 width, i32 height, Allocator allocator) {
	zeroPtr(renderer);
	renderer->pixels = alloc(width * height * sizeof(u32), 4, allocator);
}

function void
clearBuffers(Renderer* renderer, i32 width, i32 height) {
	renderer->dim.x = width;
	renderer->dim.y = height;
	i32 lastPxIndex = width * height - 1;
	for (i32 pxIndex = 0; pxIndex < lastPxIndex; pxIndex += 1) {
		renderer->pixels[pxIndex] = 0xFFFF0000;
	}
}
