#include "framework.h"
#include "resource.h"
#include "engine.h"
#include "render_os.h"

#define MAX_LOADSTRING 100

#define WIDTH 100
#define HEIGHT 50

HINSTANCE g_hInst;    
HWND g_hWnd;
int g_fps;
LPCWSTR windowClassName = L"SkiesConsoleWndClass";            

BOOL createWindow(int);
ATOM registerClass();
BOOL initInstance(int nCmdShow);
BOOL processMessages();
LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance,
					  HINSTANCE hPrevInstance,
					  LPWSTR lpCmdLine,
					  int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_hInst = hInstance;

	if (!createWindow(nCmdShow)) {
		return FALSE;
	}

	if (!(renderInit(g_hWnd, WIDTH, HEIGHT))) {
		return FALSE;
	}

	if (!gameInit()) {
		return FALSE;
	}

	LARGE_INTEGER counterFreq;
	QueryPerformanceFrequency(&counterFreq);

	int fpsCounter = 0;
	int64_t fpsDelta = 0;

	BOOL cont = TRUE;
	while (processMessages()) {
		if (IsIconic(g_hWnd)) {
			WaitMessage();
		}
		else {
			LARGE_INTEGER frameStart, frameEnd;

			QueryPerformanceCounter(&frameStart);
			gameStep();
			render();
			QueryPerformanceCounter(&frameEnd);

			int64_t deltaTicks = frameEnd.QuadPart - frameStart.QuadPart;
			++fpsCounter;
			fpsDelta += deltaTicks;
			if (fpsDelta > counterFreq.QuadPart) {
				g_fps = fpsCounter;
				fpsCounter = 0;
				fpsDelta = 0;
			}

		}
	}

	gameClean();
	renderClean();

	return 0;
}

BOOL createWindow(int nCmdShow) {
	if (!registerClass() || !initInstance(nCmdShow)) {
		return FALSE;
	}

	return TRUE;
}

ATOM registerClass()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = wndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = g_hInst;
	wcex.hIcon          = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_SKIES));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = windowClassName;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL initInstance(int nCmdShow)
{
	WCHAR szTitle[MAX_LOADSTRING];
	LoadStringW(g_hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	RECT rect = { 0, 0, WIDTH * FONT_WIDTH, HEIGHT * FONT_HEIGHT };
	AdjustWindowRect(&rect, style, FALSE);
	g_hWnd = CreateWindowW(windowClassName, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, g_hInst, nullptr);

	if (!g_hWnd) {
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

   return TRUE;
}


BOOL processMessages() {
	MSG msg;

	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			return FALSE;
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return TRUE;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		/*
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
			}
			break;
		*/
		case WM_ERASEBKGND:
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
