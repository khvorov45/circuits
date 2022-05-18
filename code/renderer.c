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
clearBuffers(Renderer* renderer, i32 width, i32 height, v4 col) {
	u32 color32 = colorToU32ARGB(col);
	renderer->dim.x = min(width, renderer->maxDim.x);
	renderer->dim.y = min(height, renderer->maxDim.y);
	i32 lastPxIndex = renderer->dim.x * renderer->dim.y - 1;
	for (i32 pxIndex = 0; pxIndex < lastPxIndex; pxIndex += 1) {
		renderer->pixels[pxIndex] = color32;
	}
}

function void
drawRect(Renderer* renderer, Rect2i rect, v4 color) {

	u32 color32 = colorToU32ARGB(color);

	v2i bottomright = getRectBottomright(rect);

	for (i32 row = rect.topleft.y; row < bottomright.y; row += 1) {
		for (i32 col = rect.topleft.x; col < bottomright.x; col += 1) {

			i32 pxIndex = row * renderer->dim.x + col;
			renderer->pixels[pxIndex] = color32;
		}
	}
}
