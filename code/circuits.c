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

	while (window->isRunning) {
		waitForInput(window);

		clearBuffers(renderer, window->dim.x, window->dim.y);

		displayPixels(window, renderer->pixels, renderer->dim.x, renderer->dim.y);
	}
}
