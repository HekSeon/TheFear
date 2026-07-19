///////////////////////////////////
// 
// particle.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "main.h" // XMFLOAT3, XMFLOAT4 ve HRESULT için gerekli[cite: 6]

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetColorParticle(int nIdxParticle, XMFLOAT4 col);