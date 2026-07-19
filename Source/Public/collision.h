///////////////////////////////////
// 
// collision.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "main.h" // BOOL, XMFLOAT3 ve XMVECTOR yapılarının sorunsuz tanınması için gerekli

//*****************************************************************************
// マクロ定義 (Macros)
//*****************************************************************************

//*****************************************************************************
// 構造体定義 (Structures)
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh, XMFLOAT3 ypos, float yw, float yh);
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2);
float dotProduct(XMVECTOR* v1, XMVECTOR* v2);
void crossProduct(XMVECTOR* ret, XMVECTOR* v1, XMVECTOR* v2);

BOOL RayCast(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 pos0, XMFLOAT3 pos1, XMFLOAT3* hit, XMFLOAT3* normal);