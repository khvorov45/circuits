typedef struct Chip {
	i32 inputCount, outputCount;
} Chip;

function void 
drawChip(Renderer* renderer, Chip chip, v2i center) {
	v2i dim = {100, 100};
	Rect2i rect = rectCenterDim(center, dim);
	v4 col = {1, 0, 0, 1};
	drawRect(renderer, rect, col);

	v2i rectBottomright = getRectBottomright(rect);

	v4 pinCol = {1, 1, 0, 1};
	i32 pinLength = 50;

	i32 pinStep = rect.dim.x / (chip.inputCount + 1);
	i32 curPinX = rect.topleft.x + pinStep; 
	for (i32 inputIndex = 0; inputIndex < chip.inputCount; inputIndex += 1) {

		drawVLine(renderer, curPinX, rectBottomright.y, rectBottomright.y + pinLength, pinCol);
		curPinX += pinStep;
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
