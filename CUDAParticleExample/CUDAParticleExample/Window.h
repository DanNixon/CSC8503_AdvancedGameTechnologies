#pragma once
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#include <Windows.h>

class Window {
public:
	Window() {
		initFake();
		init(CW_USEDEFAULT, CW_USEDEFAULT);
		choosePixelFormat();
	}
	Window(int width, int height) {
		initFake();
		init(width, height);
		choosePixelFormat();
	}
	~Window() {
		// need to clean up properly
	}

	void show() {
		ShowWindow(hwnd, SW_SHOWNORMAL);
		UpdateWindow(hwnd);
	}
	bool update() {
		MSG msg;
		bool result = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0;
		//if (result) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		//}
		return result;
	}

	void swapBuffers() {
		SwapBuffers(GetDC(hwnd));
	}

protected:
	static WNDCLASSEX wc;
	static HINSTANCE hInstance;
	static const char className[];

	HWND hwnd;
	PIXELFORMATDESCRIPTOR pfd;

private:
	Window(int fake) {
		init(CW_USEDEFAULT, CW_USEDEFAULT);
	}
	void init(int width, int height);
	void initFake();
	void choosePixelFormat();

	static WNDCLASSEX createWindowClass();
	friend LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};