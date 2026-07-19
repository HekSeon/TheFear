///////////////////////////////////
// 
// sprite.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "main.h" // ID3D11Buffer ve XMFLOAT4 tiplerinin tanınması için gerekli

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************
void SetSprite(ID3D11Buffer* buf, float X, float Y, float Width, float Height, float U, float V, float UW, float VH);

void SetSpriteColor(ID3D11Buffer* buf, float X, float Y, float Width, float Height, float U, float V, float UW, float VH, XMFLOAT4 color);

void SetSpriteColorRotation(ID3D11Buffer* buf, float X, float Y, float Width, float Height,
	float U, float V, float UW, float VH,
	XMFLOAT4 Color, float Rot);

void SetSpriteLeftTop(ID3D11Buffer* buf, float X, float Y, float Width, float Height, float U, float V, float UW, float VH);