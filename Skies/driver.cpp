#include "framework.h"
#include "resource.h"
#include "Engine.h"

#define MAX_LOADSTRING 100

HINSTANCE g_hInst;                                
LPCWSTR windowClassName = L"SkiesConsoleWndClass";            

BOOL createWindow(int);
ATOM registerClass();
BOOL initInstance(int nCmdShow);
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

	if (!gameInit()) {
		return FALSE;
	}

	MSG msg;
	BOOL cont = TRUE;
	while (cont) {
		gameStep();
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				cont = FALSE;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}		
		}
	}

	gameClean();

	return (int)msg.wParam;
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

	HWND hWnd = CreateWindowW(windowClassName, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, g_hInst, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			TextOutW(hdc, 10, 10, L"Hello, skies!", 13);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
