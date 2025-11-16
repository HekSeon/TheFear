//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : 
//
//=============================================================================
#pragma once

#include "model.h"
#include <DirectXMath.h>
using namespace DirectX;



//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define MAX_ENEMY		(100)
#define	ENEMY_SIZE		(5.0f)				// 当たり判定の大きさ


struct ENEMY
{
    XMFLOAT4X4 mtxWorld;
    XMFLOAT3 pos;
    XMFLOAT3 rot;
    XMFLOAT3 scl;

    BOOL use;
    BOOL load;
    DX11_MODEL model;
    XMFLOAT4 diffuse[MODEL_MAX_MATERIAL] = {};

    float spd;
    float dir;
    float size;
    int shadowIdx;

    ENEMY* parent;

    float time;
    int tblNo;
    int tblMax;

   

    float health;
    bool isAlive;
    int targetEnemy;
};





//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

void SpawnEnemy();

ENEMY *GetEnemy(void);

void DealDamageToEnemy();
ENEMY* FindNearestEnemy(XMFLOAT3 swordPos);