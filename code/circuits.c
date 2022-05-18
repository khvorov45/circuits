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

		Rect2i tempRect = {{10, 50}, {90, 50}};
		v4 tempRectCol = {0, 0, 1, 1};
		drawRect(renderer, tempRect, tempRectCol);

		v4 tempVLineCol = {0, 1, 1, 1};
		drawVLine(renderer, 100, -1000, 100000, tempVLineCol);

		v4 tempHLineCol = {1, 0, 1, 1};
		drawHLine(renderer, 100, -1000, 100000, tempHLineCol);

		displayPixels(window, renderer->pixels, renderer->dim.x, renderer->dim.y);
	}
}
