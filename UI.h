#pragma once
#include "main.h"

#define SCORE_MAX			(99999)		// スコアの最大値
#define HEALT_DIGIT			(3)			// 桁数


//////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////
HRESULT InitUI(void);
void UninitUI(void);
void UpdateUI(void);
void DrawUI(void);
int GetUI(void);