#pragma once
#include "framework.h"

const int FONT_WIDTH = 8;
const int FONT_HEIGHT = 8;

bool renderInit(HWND hWnd, int width, int height);
void render();
void renderClean();