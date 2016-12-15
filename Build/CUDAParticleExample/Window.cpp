#include "Window.h"
#include <GL\glew.h>
#include <GL\wglew.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

WNDCLASSEX Window::wc = Window::createWindowClass();
HINSTANCE Window::hInstance = GetModuleHandle(NULL);
const char Window::className[] = "OpenGLWindowClass";

void Window::init(int width, int height)
{
  hwnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_CLIENTEDGE, className, "OpenGL",
                        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
  if (!hwnd)
  {
    MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    exit(0);
  }
  SetWindowLong(hwnd, GWL_USERDATA, (LONG)this); // possibly needs fixing, changed to SetWindowLongPtr

  RECT client, window;
  GetClientRect(hwnd, &client);
  GetWindowRect(hwnd, &window);
  MoveWindow(hwnd, window.left, window.top, width + (window.right - client.right) - (window.left - client.left),
             height + (window.bottom - client.bottom) - (window.top - client.top), true);
}

WNDCLASSEX Window::createWindowClass()
{
  WNDCLASSEX wc;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_OWNDC | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = sizeof(Window *);
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = className;
  wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

  if (!RegisterClassEx(&wc))
  {
    MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    exit(0);
  }

  return wc;
}

void Window::initFake()
{
  Window fake(0);
  HDC hDC = GetDC(fake.hwnd);
  SetPixelFormat(hDC, 1, &fake.pfd);
  HGLRC hRC = wglCreateContext(hDC);
  wglMakeCurrent(hDC, hRC);

  if (hDC == 0 || hRC == 0)
  {
    MessageBox(NULL, "OpenGL Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    exit(0);
  }

  ///*
  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    MessageBox(NULL, "Error initializing GLEW!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    exit(0);
  }
  const GLubyte *version = glGetString(GL_VERSION);
  printf("OpenGL Version: %s.\n", version);

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(hRC);
  ReleaseDC(fake.hwnd, hDC);
  DestroyWindow(fake.hwnd);
}

void Window::choosePixelFormat()
{
  HDC hDC = GetDC(hwnd);
  int nPixelFormat = 0;
  int nPixCount = 0;
  int pixAttribs[] = {WGL_SUPPORT_OPENGL_ARB,
                      GL_TRUE,
                      WGL_DRAW_TO_WINDOW_ARB,
                      GL_TRUE,
                      WGL_ACCELERATION_ARB,
                      WGL_FULL_ACCELERATION_ARB,
                      WGL_DOUBLE_BUFFER_ARB,
                      GL_TRUE,
                      WGL_SAMPLE_BUFFERS_ARB,
                      GL_TRUE,
                      WGL_COLOR_BITS_ARB,
                      32,
                      WGL_DEPTH_BITS_ARB,
                      24,
                      WGL_SAMPLES_ARB,
                      8,
                      WGL_PIXEL_TYPE_ARB,
                      WGL_TYPE_RGBA_ARB,
                      0};
  wglChoosePixelFormatARB(hDC, pixAttribs, NULL, 1, &nPixelFormat, (UINT *)&nPixCount);
  if (nPixCount == 0)
  {
    MessageBox(NULL, "OpenGL Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    exit(0);
  }
  SetPixelFormat(hDC, nPixelFormat, &pfd);
  const int attribs[] = {WGL_CONTEXT_MAJOR_VERSION_ARB, 3, WGL_CONTEXT_MINOR_VERSION_ARB, 3, 0};
  HGLRC hRC = wglCreateContextAttribsARB(hDC, NULL, attribs);
  if (hRC == NULL)
  {
    MessageBox(NULL, "OpenGL Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    exit(0);
  }
  wglMakeCurrent(hDC, hRC);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Window *window = (Window *)GetWindowLong(hwnd, GWL_USERDATA);
  switch (msg)
  {
  case WM_CLOSE:
    DestroyWindow(hwnd);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}