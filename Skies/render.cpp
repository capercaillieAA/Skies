#include "framework.h"
#include "render.h"
#include "render_os.h"
#include <gdiplus.h>

struct Symbol {
	char ch;
	color_t fg;
	color_t bg;
};

struct RenderContext {
	HWND hWnd;
	HBITMAP hOffscreenBuffer;
	HFONT hFont;

	int width;
	int height;
	Symbol *screenBuffer;
};

static RenderContext* pCurrentCtx = nullptr;

#define NUM_OF_ELEMS(arr) (sizeof(arr) / sizeof(arr[0]))

static const COLORREF terminalColors[] = {
	RGB(1, 1, 1),
	RGB(222, 56, 43),
	RGB(57, 181, 74),
	RGB(255, 199, 6),
	RGB(0, 111, 84),
	RGB(118, 38, 113),
	RGB(44, 181, 233),
	RGB(204, 204, 204),
	RGB(128, 128, 128),
	RGB(255, 0, 0),
	RGB(0, 255, 0),
	RGB(255, 255, 0),
	RGB(0, 0, 255),
	RGB(255, 0, 255),
	RGB(0, 255, 255),
	RGB(255, 255, 255)
};

int putCh(int x, int y, char ch, color_t fg, color_t bg) {
	assert(pCurrentCtx != nullptr);
	assert(x >= 0 && x <= pCurrentCtx->width);
	assert(y >= 0 && y <= pCurrentCtx->height);

	Symbol *pSymb = &pCurrentCtx->screenBuffer[y * pCurrentCtx->width + x];
	pSymb->ch = ch;
	pSymb->fg = fg;
	pSymb->bg = bg;

	return 0;
}

int putStr(int x, int y, const char* str, color_t fg, color_t bg) {
	assert(pCurrentCtx != nullptr);
	assert(x >= 0 && x <= pCurrentCtx->width);
	assert(y >= 0 && y <= pCurrentCtx->height);
	assert(str != nullptr);

	Symbol* pSymb = &pCurrentCtx->screenBuffer[y * pCurrentCtx->width + x];
	while (char ch = *(str++)) {
		pSymb->ch = ch;
		pSymb->fg = fg;
		pSymb->bg = bg;
		pSymb++;
	}

	return 0;
}

bool renderInit(HWND hWnd, int width, int height) {
	pCurrentCtx = new RenderContext;

	pCurrentCtx->hWnd = hWnd;

	HDC hDC = GetDC(pCurrentCtx->hWnd);
	pCurrentCtx->hOffscreenBuffer = CreateCompatibleBitmap(hDC, width * FONT_WIDTH, height * FONT_HEIGHT);
	ReleaseDC(pCurrentCtx->hWnd, hDC);
	
	pCurrentCtx->hFont = CreateFontW(FONT_WIDTH, FONT_HEIGHT, 0, 0, 0, 0, 0, 0, OEM_CHARSET, OUT_DEVICE_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Terminal");
	
	int bufferSize = width * height;
	pCurrentCtx->width = width;
	pCurrentCtx->height = height;
	pCurrentCtx->screenBuffer = new Symbol[bufferSize];
	for (int i = 0; i < bufferSize; ++i) {
		pCurrentCtx->screenBuffer[i] = Symbol{' ', COLOR_RED, COLOR_WHITE};
	}

	return true;
}

void renderClean() {	
	DeleteObject(pCurrentCtx->hFont);
	DeleteObject(pCurrentCtx->hOffscreenBuffer);

	delete[] pCurrentCtx->screenBuffer;
	delete pCurrentCtx;

	pCurrentCtx = nullptr;
}

extern int g_fps;

static void renderFps(HDC hdc) {
	char buf[40] = { 0 };
	snprintf(buf, 40, "%d FPS", g_fps);

	SetTextColor(hdc, terminalColors[COLOR_WHITE]);
	SetBkColor(hdc, terminalColors[COLOR_BLACK]);
	TextOutA(hdc, 10, 10, buf, strlen(buf));
}

static void renderScreen(HDC hdc) {
	for (int j = 0; j < pCurrentCtx->height; ++j) {
		for (int i = 0; i < pCurrentCtx->width; ++i) {
			int pos = j * pCurrentCtx->width + i;
			int fg = pCurrentCtx->screenBuffer[pos].fg;
			int bg = pCurrentCtx->screenBuffer[pos].bg;
			char symbol = pCurrentCtx->screenBuffer[pos].ch;

			assert(fg >= 0 && fg < NUM_OF_ELEMS(terminalColors));
			assert(bg >= 0 && bg < NUM_OF_ELEMS(terminalColors));

			SetTextColor(hdc, terminalColors[fg]);
			SetBkColor(hdc, terminalColors[bg]);
			TextOutA(hdc, i * FONT_WIDTH, j * FONT_HEIGHT, &symbol, 1);
		}
	}
}

void render() {
	RECT rect;
	GetClientRect(pCurrentCtx->hWnd, &rect);
	
	HDC hDC = GetDC(pCurrentCtx->hWnd);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hMemBitmap = (HBITMAP)SelectObject(hMemDC, pCurrentCtx->hOffscreenBuffer);
	HFONT hMemFont = (HFONT)SelectObject(hMemDC, pCurrentCtx->hFont);
	
	renderScreen(hMemDC);

	SelectObject(hMemDC, hMemFont);
	renderFps(hMemDC);

	BitBlt(hDC, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hMemBitmap);
	ReleaseDC(pCurrentCtx->hWnd, hMemDC);
	ReleaseDC(pCurrentCtx->hWnd, hDC);
}