///////////////////////////////////
// 
// score.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "main.h" // HRESULT yapısı için gerekli

//*****************************************************************************
// マクロ定義 (Macros)
//*****************************************************************************
#define SCORE_MAX			(99999)		// スコアの最大値
#define SCORE_DIGIT			(5)			// 桁数
#define	ADD_SCORE_HITOBJ	(10)
#define	ADD_SCORE_HITBULLET	(100)

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

void AddScore(int add);
int GetScore(void);