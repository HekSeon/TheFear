///////////////////////////////////
// 
// debugproc.h
// 
// Author: HekSeon
///////////////////////////////////
#pragma once


#include "main.h"

///////////////////////////////////
// protota area
///////////////////////////////////
void DrawDebugProc(void);

bool IsGameDebugOpen(void);
void SetGameDebugOpen(bool open);
void HandleGameDebugClick(HWND window, int x, int y);
void DrawGameDebugGeometry(void);
void ReleaseGameDebugGeometry(void);
void DrawGameOverlay(HDC dc);

void PrintDebugProc(char* fmt, ...);
