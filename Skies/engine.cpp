#include "engine.h"
#include "render.h"

bool gameInit() {
	return true;
}

void gameStep() {
	putStr(10, 30, "Hello, world!", COLOR_YELLOW, COLOR_BLACK);
}

void gameClean() {

}