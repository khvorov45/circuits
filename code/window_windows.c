typedef struct PlatformWindow {
	HWND hwnd;
	HDC hdc;
	BITMAPINFO bmi;
	b32 inputModified;
} PlatformWindow;

#include "window.c"

function LRESULT CALLBACK
windowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
	LRESULT result = 0;

	AppWindow* window = (AppWindow*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch (message) {
	case WM_CLOSE: case WM_DESTROY:
		window->isRunning = false;
		window->platform.inputModified = true;
	}

	result = DefWindowProcW(hwnd, message, wparam, lparam);
	return result;
}

function b32
initWindow(AppWindow* window, i32 width, i32 height) {
	b32 success = false;
	zeroPtr(window);

	WNDCLASSEXW windowClass = {
		.cbSize = sizeof(windowClass),
		.lpfnWndProc = &windowProc,
		.hInstance = GetModuleHandleW(NULL),
		.hIcon = LoadIconA(NULL, IDI_APPLICATION),
		.hCursor = LoadCursorA(NULL, IDC_ARROW),
		.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH),
		.lpszClassName = L"circuitsClass",
	};

	if (RegisterClassExW(&windowClass)) {

		v2i dim = {width, height};

		HWND hwnd = CreateWindowExW(
			WS_EX_APPWINDOW, windowClass.lpszClassName, L"circuits", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, dim.x, dim.y,
			0, 0, windowClass.hInstance, 0
		);

		if (IsWindow(hwnd)) {

			ShowWindow(hwnd, SW_SHOWMINIMIZED);
			ShowWindow(hwnd, SW_SHOWNORMAL);

			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)window);

			window->dim = dim;
			window->isRunning = true;

			window->platform.hwnd = hwnd;
			window->platform.hdc = GetDC(hwnd);

			window->platform.bmi.bmiHeader.biSize = sizeof(window->platform.bmi.bmiHeader);
			window->platform.bmi.bmiHeader.biWidth = width;
			window->platform.bmi.bmiHeader.biHeight = -height; // NOTE(khvorov) Negative = top-down
			window->platform.bmi.bmiHeader.biPlanes = 1;
			window->platform.bmi.bmiHeader.biBitCount = 32;
			window->platform.bmi.bmiHeader.biCompression = BI_RGB;

			success = true;
		}
	}

	return success;
}

function void
waitForInput(AppWindow* window) {

	window->platform.inputModified = false;

	for (;;) {
		MSG msg = {0};

		if (!window->platform.inputModified) {
			GetMessageW(&msg, 0, 0, 0);
		} else if (!PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
			break;
		}

		switch (msg.message) {
		case WM_PAINT:
			window->platform.inputModified = true;

		default: {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} break;
		}

	}
}

function void
displayPixels(AppWindow* window, void* pixels, i32 pixelsWidth, i32 pixelsHeight) {
	window->platform.bmi.bmiHeader.biWidth = pixelsWidth;
	window->platform.bmi.bmiHeader.biHeight = -pixelsHeight; // NOTE(khvorov) Negative = top-down
	StretchDIBits(
		window->platform.hdc,
		0, 0, window->dim.x, window->dim.y,
		0, 0, pixelsWidth, pixelsHeight,
		pixels, &window->platform.bmi,
		DIB_RGB_COLORS, SRCCOPY
	);
}
