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
	ULONG_PTR gdiToken;

	int width;
	int height;
	Symbol *screenBuffer;
};

static RenderContext* pCurrentCtx = nullptr;


int putCh(int x, int y, char ch, color_t fg, color_t bg) {
	assert(pCurrentCtx != nullptr);
	assert(x >= 0 && x <= pCurrentCtx->width);
	assert(y >= 0 && y <= pCurrentCtx->height);

	Symbol* pSymb = &pCurrentCtx->screenBuffer[y * pCurrentCtx->width + x];
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
	pCurrentCtx->width = width;
	pCurrentCtx->height = height;
	pCurrentCtx->screenBuffer = new Symbol[width * height];

	Gdiplus::GdiplusStartupInput startupInput;
	Gdiplus::GdiplusStartup(&pCurrentCtx->gdiToken, &startupInput, NULL);

	return true;
}

void render() {
	HDC hdc = GetDC(pCurrentCtx->hWnd);
	Gdiplus::Graphics graphics(hdc);
	graphics.Clear(Gdiplus::Color::AliceBlue);
}

void renderClean() {
	Gdiplus::GdiplusShutdown(pCurrentCtx->gdiToken);

	delete[] pCurrentCtx->screenBuffer;
	delete pCurrentCtx;

	pCurrentCtx = nullptr;
}