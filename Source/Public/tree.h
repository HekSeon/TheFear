///////////////////////////////////
// 
// tree.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "main.h" // HRESULT, XMFLOAT3 ve XMFLOAT4 tiplerinin tanınması için gerekli

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************
HRESULT InitTree(void);
void UninitTree(void);
void UpdateTree(void);
void DrawTree(void);

int SetTree(const XMFLOAT3& pos, const XMFLOAT4& color);