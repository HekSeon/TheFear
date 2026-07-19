///////////////////////////////////
// 
// game.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "main.h" // HRESULT ve BOOL yapılarının tanınması için gerekli

static BOOL	g_bPause = TRUE;	// ポーズON/OFF

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************
HRESULT InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);