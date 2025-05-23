//=============================================================================
//
// モデル処理 [player.h]
// Author : 
//
//=============================================================================
#pragma once

#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER		(1)					// プレイヤーの数

#define	PLAYER_SIZE		(5.0f)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		oldpos;
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス

	BOOL			load;
	DX11_MODEL		model;		// モデル情報

	float			spd;		// 移動スピード
	float			dir;		// 向き
	float			size;		// 当たり判定の大きさ
	int				shadowIdx;	// 影のIndex
	BOOL			use;
	BOOL			move;
	BOOL			attack;

	// 階層アニメーション用のメンバー変数
	float			time;		// 線形補間用
	int				tblNo;		// 行動データのテーブル番号
	int				tblMax;		// そのテーブルのデータ数


	// 親は、NULL、子供は親のアドレスを入れる(セーブ＆ロードの時は↓ここ気をつける事)
	PLAYER			*parent;	// 自分が親ならNULL、自分が子供なら親のplayerアドレス
	int				 currentTblAdr;

	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// 自分が立っている所
};


#define	MODEL_PLAYER		"data/MODEL/bear/body.obj"	// 読み込むモデル名

#define	MODEL_PLAYER_HEAD	"data/MODEL/bear/head.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_LARM	"data/MODEL/bear/leftarm.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_RARM	"data/MODEL/bear/rightarm.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_LLEG	"data/MODEL/bear/leftleg.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_RLEG	"data/MODEL/bear/rightleg.obj"	// 読み込むモデル名

#define PLAYER_PARTS_MAX	(5)								// プレイヤーのパーツの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

void playtpsanimation(void);
void playfpsanimation(void);

void playattackanimation(void);

