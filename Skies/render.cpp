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
	Gdiplus::Image *pFontText;
	Symbol *screenBuffer;
};

static RenderContext* pCurrentCtx = nullptr;

static LPCWSTR fontFileName = L".\\assets\\terminal8x8.png";

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
	ULONG gdiToken;
	Gdiplus::GdiplusStartupInput startupInput;
	if (Gdiplus::GdiplusStartup(&gdiToken, &startupInput, NULL) != Gdiplus::Status::Ok) {
		return false;
	}
	
	pCurrentCtx = new RenderContext;
	pCurrentCtx->hWnd = hWnd;
	pCurrentCtx->gdiToken = gdiToken;
	pCurrentCtx->width = width;
	pCurrentCtx->height = height;
	pCurrentCtx->screenBuffer = new Symbol[width * height];
	pCurrentCtx->pFontText = new Gdiplus::Image(fontFileName); //todo: check

	return true;
}

void render() {
	RECT rect;
	GetClientRect(pCurrentCtx->hWnd, &rect);
	Gdiplus::Bitmap memBitmap(rect.right, rect.bottom);
	Gdiplus::Graphics gBitmap(&memBitmap);
	
	gBitmap.Clear(Gdiplus::Color::AliceBlue);
	Gdiplus::RectF bounds(0, 0, float(pCurrentCtx->width * FONT_WIDTH), float(pCurrentCtx->height * FONT_HEIGHT));
	gBitmap.DrawImage(pCurrentCtx->pFontText, bounds);
	
	HDC hdc = GetDC(pCurrentCtx->hWnd);
	Gdiplus::Graphics gHdc(hdc);
	gHdc.DrawImage(&memBitmap, 0, 0);
	//BitBlt(hdc, 0, 0, rect.right, rect.bottom, gBitmap.GetHDC(), 0, 0, SRCCOPY);
	gHdc.ReleaseHDC(hdc);
}

void renderClean() {
	Gdiplus::GdiplusShutdown(pCurrentCtx->gdiToken);

	delete[] pCurrentCtx->screenBuffer;
	delete pCurrentCtx;

	pCurrentCtx = nullptr;
}