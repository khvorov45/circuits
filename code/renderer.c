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
drawPixel(Renderer* renderer, i32 xCoord, i32 yCoord, u32 color) {
	if (yCoord < renderer->dim.y && xCoord < renderer->dim.x && xCoord >= 0 && yCoord >= 0) {
		renderer->pixels[yCoord * renderer->dim.x + xCoord] = color;
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
drawHLine(Renderer* renderer, i32 yCoord, i32 xStart, i32 xEnd, u32 color) {
	if (xStart > xEnd) {
		i32 temp = xStart;
		xStart = xEnd;
		xEnd = temp;
	}

	if (xStart < renderer->dim.x && xEnd >= 0 && yCoord > 0 && yCoord < renderer->dim.y) {
		xStart = max(xStart, 0);
		xEnd = min(xEnd, renderer->dim.x - 1);

		for (i32 xCoord = xStart; xCoord <= xEnd; xCoord += 1) {
			renderer->pixels[yCoord * renderer->dim.x + xCoord] = color;
		}
	}
}

function void
drawCircle(Renderer* renderer, v2i center, i32 radius, v4 color) {

	u32 color32 = colorToU32ARGB(color);

	i32 curX = radius;
	i32 curY = 0;
	i32 curMidError = 1 - radius;
	
	while (curX > curY) {
		drawHLine(renderer, center.y + curY, center.x - curX, center.y + curX, color32);
		drawHLine(renderer, center.y - curY, center.x - curX, center.y + curX, color32);
		drawHLine(renderer, center.y - curX, center.x - curY, center.y + curY, color32);
		drawHLine(renderer, center.y + curX, center.x - curY, center.y + curY, color32);
		
		curY += 1;

		if (curMidError <= 0) {
			curMidError += 2 * curY + 1;
		} else {
			curX -= 1;
			curMidError += 2 * curY - 2 * curX + 1;
		}
	}
}
