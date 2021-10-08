#include <cassert>
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

int putStr(int x, int y, const char* ch, color_t fg, color_t bg) {
	return 0;
}

bool renderInit(HWND hWnd, int width, int height) {
	pCurrentCtx = new RenderContext;

	pCurrentCtx->hWnd = hWnd;

	HDC hDC = GetDC(pCurrentCtx->hWnd);
	pCurrentCtx->hOffscreenBuffer = CreateCompatibleBitmap(hDC, width * FONT_WIDTH, height * FONT_HEIGHT);
	ReleaseDC(pCurrentCtx->hWnd, hDC);
	
	pCurrentCtx->hFont = CreateFontW(-FONT_WIDTH, -FONT_HEIGHT, 0, 0, 0, 0, 0, 0, OEM_CHARSET, OUT_DEVICE_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Terminal");
	
	int bufferSIze = width * height;
	pCurrentCtx->width = width;
	pCurrentCtx->height = height;
	pCurrentCtx->screenBuffer = new Symbol[bufferSIze];
	for (int i = 0; i < bufferSIze; i++) {
		pCurrentCtx->screenBuffer[i] = Symbol{};
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

void render() {
	RECT rect;
	GetClientRect(pCurrentCtx->hWnd, &rect);
	
	HDC hDC = GetDC(pCurrentCtx->hWnd);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hMemBitmap = (HBITMAP)SelectObject(hMemDC, pCurrentCtx->hOffscreenBuffer);
	
	TextOutW(hMemDC, 0, 0, L"Hello, World!", 13);

	BitBlt(hDC, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hMemBitmap);
	ReleaseDC(pCurrentCtx->hWnd, hMemDC);
	ReleaseDC(pCurrentCtx->hWnd, hDC);
}