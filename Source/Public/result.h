#pragma once

#include "main.h"

void SetResultStats(int wave, int score);
void UpdateResult(void);
void HandleResultClick(HWND window, int x, int y);
void DrawResult(HDC dc);
void ShowLoadingScreen(void);
