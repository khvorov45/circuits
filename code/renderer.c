typedef struct Renderer {
	u32* pixels;
	v2i dim;
	v2i maxDim;
} Renderer;

function Rect2i
clipRectToRenderBounds(Renderer* renderer, Rect2i rect) {
	Rect2i renderBounds = {{0, 0}, renderer->dim};
	Rect2i result = clipRectToRect(rect, renderBounds);	
	return result;
}

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
drawRect(Renderer* renderer, Rect2i rectInit, v4 color) {

	u32 color32 = colorToU32ARGB(color);

	Rect2i rect = clipRectToRenderBounds(renderer, rectInit);
	v2i bottomright = getRectBottomright(rect);

	for (i32 row = rect.topleft.y; row < bottomright.y; row += 1) {
		for (i32 col = rect.topleft.x; col < bottomright.x; col += 1) {

			i32 pxIndex = row * renderer->dim.x + col;
			renderer->pixels[pxIndex] = color32;
		}
	}
}

function void
drawVLine(Renderer* renderer, i32 xCoord, i32 y1, i32 y2, v4 color) {

	if (xCoord > 0 && xCoord < renderer->dim.x) {
		y1 = clamp(y1, 0, renderer->dim.y);
		y2 = clamp(y2, 0, renderer->dim.y);

		if (y1 > y2) {
			i32 temp = y1;
			y1 = y2;
			y2 = temp;
		}

		u32 color32 = colorToU32ARGB(color);
		for (i32 row = y1; row < y2; row += 1) {
			i32 pxIndex = row * renderer->dim.x + xCoord;
			renderer->pixels[pxIndex] = color32;
		}
	}
}

function void
drawHLine(Renderer* renderer, i32 yCoord, i32 x1, i32 x2, v4 color) {

	if (yCoord > 0 && yCoord < renderer->dim.y) {
		x1 = clamp(x1, 0, renderer->dim.x);
		x2 = clamp(x2, 0, renderer->dim.x);

		if (x1 > x2) {
			i32 temp = x1;
			x1 = x2;
			x2 = temp;
		}

		u32 color32 = colorToU32ARGB(color);
		for (i32 col = x1; col < x2; col += 1) {
			i32 pxIndex = yCoord * renderer->dim.x + col;
			renderer->pixels[pxIndex] = color32;
		}
	}
}
