///////////////////////////////////
// 
// sound.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include <windows.h>
#include "xaudio2.h"                        // サウンド処理で必要

//*****************************************************************************
// サウンドファイル (Sound Labels)
//*****************************************************************************
enum
{
	SOUND_LABEL_BGM_sample000,	// BGM0
	SOUND_LABEL_BGM_sample001,	// BGM1
	SOUND_LABEL_BGM_sample002,	// BGM2
	SOUND_LABEL_SE_bomb000,		// 爆発音
	SOUND_LABEL_SE_defend000,	// 防御音
	SOUND_LABEL_SE_defend001,	// 防御音
	SOUND_LABEL_SE_hit000,		// ヒット音
	SOUND_LABEL_SE_laser000,	// レーザー音
	SOUND_LABEL_SE_lockon000,	// ロックオン音
	SOUND_LABEL_SE_shot000,		// ショット音
	SOUND_LABEL_SE_shot001,		// ショット音
	SOUND_LABEL_SE_shot,		// ヒット音
	SOUND_LABEL_BGM_title,		// タイトルBGM
	SOUND_LABEL_BGM_ingame,		// インゲームBGM
	SOUND_LABEL_SE_hit,			// ヒット音
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);