///////////////////////////////////
// 
// shadow.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "main.h" // HRESULT, XMFLOAT3 ve XMFLOAT4 tiplerinin tanınması için gerekli

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************
HRESULT InitShadow(void);
void UninitShadow(void);
void UpdateShadow(void);
void DrawShadow(void);

int CreateShadow(XMFLOAT3 pos, float fSizeX, float fSizeZ);
void ReleaseShadow(int nIdxShadow);
void SetPositionShadow(int nIdxShadow, XMFLOAT3 pos);
void SetColorShadow(int nIdxShadow, XMFLOAT4 col);