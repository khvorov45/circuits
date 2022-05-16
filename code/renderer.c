typedef struct Renderer {
	u32* pixels;
	v2i dim;
	v2i maxDim;
} Renderer;

function void
initRenderer(Renderer* renderer, i32 maxwidth, i32 maxheight, Allocator allocator) {
	zeroPtr(renderer);
	renderer->pixels = alloc(maxwidth * maxheight * sizeof(u32), 4, allocator);
	renderer->maxDim.x = maxwidth;
	renderer->maxDim.y = maxheight;
}

function void
clearBuffers(Renderer* renderer, i32 width, i32 height) {
	renderer->dim.x = min(width, renderer->maxDim.x);
	renderer->dim.y = min(height, renderer->maxDim.y);
	i32 lastPxIndex = renderer->dim.x * renderer->dim.y - 1;
	for (i32 pxIndex = 0; pxIndex < lastPxIndex; pxIndex += 1) {
		renderer->pixels[pxIndex] = 0xFFFF0000;
	}
}
