#pragma once
#include "main.h"

#define SCORE_MAX			(99999)		// �X�R�A�̍ő�l
#define HEALT_DIGIT			(3)			// ����


//////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////
HRESULT InitUI(void);
void UninitUI(void);
void UpdateUI(void);
void DrawUI(void);
int GetUI(void);