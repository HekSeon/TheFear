///////////////////////////////////
// 
// fade.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "main.h" // HRESULT yapısı için gerekli

//*****************************************************************************
// マクロ定義 (Macros)
//*****************************************************************************

// フェードの状態 (Fade States)
typedef enum
{
	FADE_NONE = 0,		// 何もない状態
	FADE_IN,			// フェードイン処理
	FADE_OUT,			// フェードアウト処理
	FADE_MAX
} FADE;

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************
HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(FADE fade, int modeNext);
FADE GetFade(void);