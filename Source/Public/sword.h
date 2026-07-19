///////////////////////////////////
// 
// sword.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "main.h" // HRESULT, XMFLOAT3 ve XMFLOAT4X4 tiplerinin güvenle tanınması için
#include "model.h"
#include "dissolve.h"

//*****************************************************************************
// マクロ定義 (Macros)
//*****************************************************************************
#define	SWORD_SIZE		(5.0f)				// 当たり判定の大きさ

//*****************************************************************************
// 構造体定義 (Structures)
//*****************************************************************************
struct SWORD
{
	XMFLOAT3		pos;		// モデルの位置
	XMFLOAT3		oldpos;
	XMFLOAT3		rot;		// モデルの向き(回転)
	XMFLOAT3		scl;		// モデルの大きさ(スケール)

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス

	BOOL			load;
	DX11_MODEL		model;		// モデル情報

	float			dir;		// 向き
	float			size;		// 当たり判定の大きさ
	int				shadowIdx;	// 影のインデックス番号
	BOOL			use;

	// アニメーション用変数
	float			time;		// 線形補間用
	int				tblNo;		// 行動データのテーブル番号
	int				tblMax;		// そのテーブルのデータ数

	DISSOLVE_STATE dissolve;
};

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************
HRESULT InitSword(void);
void UninitSword(void);
void UpdateSword(void);
void DrawSword(void);
SWORD* GetSword(void);
void SetSwordPos(XMFLOAT3 offset, float rotX, float rotY, float rotZ);
void AttackNearestEnemy();