﻿//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "sword.h"
#include "meshfield.h"
#include "collision.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	MODEL_PLAYER		"data/MODEL/player.obj"			// 読み込むモデル名





#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(16.0f)							// プレイヤーの足元をあわせる


	

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************




//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用


static BOOL g_Load = FALSE;

ENEMY* enemies = NULL;



// プレイヤーの階層アニメーションデータ
// プレイヤーの左右パーツを動かしているアニメデータ


	static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
				{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 }, // Başlangıç pozisyonu
				{ XMFLOAT3(0.0f, 0.1f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.05f, 1.0f), 60 }, // Nefes alırken hafifçe yukarı ve genişleme
				{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 90 }, // Geri dönüş

	};

	static INTERPOLATION_DATA move_tbl_larm[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 9, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(-XM_PI / 9.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	};

	static INTERPOLATION_DATA move_tbl_rarm[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(-XM_PI / 9, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 9.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

	};

	static INTERPOLATION_DATA move_tbl_lleg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 9.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 9, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

	};

	static INTERPOLATION_DATA move_tbl_rleg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 9, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 9.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

	};


	static INTERPOLATION_DATA idle_tbl_head[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 }, // Başlangıç pozisyonu
		{ XMFLOAT3(0.0f, 0.1f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.05f, 1.0f), 60 }, // Nefes alırken hafifçe yukarı ve genişleme
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 90 }, // Geri dönüş

	};

	static INTERPOLATION_DATA idle_tbl_larm[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	};

	static INTERPOLATION_DATA idle_tbl_rarm[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

	};

	static INTERPOLATION_DATA idle_tbl_lleg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

	};

	static INTERPOLATION_DATA idle_tbl_rleg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	};
	static INTERPOLATION_DATA* g_IdleTblAdr[] =
	{
		idle_tbl_head,
		idle_tbl_larm,
		idle_tbl_rarm,
		idle_tbl_lleg,
		idle_tbl_rleg,
	};

	static INTERPOLATION_DATA* g_MoveTblAdr[] =
	{
		move_tbl_head,
		move_tbl_larm,
		move_tbl_rarm,
		move_tbl_lleg,
		move_tbl_rleg,

	};



	/* fps animation*/

	static INTERPOLATION_DATA Fidle_tbl_larm[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 3.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 3.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	};

	static INTERPOLATION_DATA Fidle_tbl_rarm[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 3.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 3.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

	};

	static INTERPOLATION_DATA Fidle_tbl_lleg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

	};

	static INTERPOLATION_DATA Fidle_tbl_rleg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	};

	static INTERPOLATION_DATA Fmove_tbl_larm[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	};

	static INTERPOLATION_DATA Fmove_tbl_rarm[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

	};


	static INTERPOLATION_DATA Fmove_tbl_lleg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 9.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 9, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

	};

	static INTERPOLATION_DATA Fmove_tbl_rleg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 9, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 9.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

	};




	static INTERPOLATION_DATA* g_FIdleTblAdr[] =
	{
		Fidle_tbl_larm,
		Fidle_tbl_rarm,
		Fidle_tbl_lleg,
		Fidle_tbl_rleg
	};

	static INTERPOLATION_DATA* g_FMoveTblAdr[] =
	{
		Fmove_tbl_larm,
		Fmove_tbl_rarm,
		Fmove_tbl_lleg,
		Fmove_tbl_rleg

	};






//=============================================================================
// 初期化処理
//=============================================================================
	HRESULT InitPlayer(void)
	{
		LoadModel(MODEL_PLAYER, &g_Player.model);
		g_Player.load = TRUE;

		g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, 0.0f };
		g_Player.rot = { 0.0f, 0.0f, 0.0f };
		g_Player.scl = { 0.5f, 0.5f, 0.5f };

		g_Player.spd = 0.0f;			// 移動スピードクリア
		g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

		g_Player.use = TRUE;

		// ここでプレイヤー用の影を作成している
		XMFLOAT3 pos = g_Player.pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
		//          ↑
		//        このメンバー変数が生成した影のIndex番号



		// 階層アニメーション用の初期化処理
		g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる

		// パーツの初期化
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			g_Parts[i].use = FALSE;

			// 位置・回転・スケールの初期設定
			g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// 親子関係
			g_Parts[i].parent = &g_Player;		// ← ここに親のアドレスを入れる
			//	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
			//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

				// 階層アニメーション用のメンバー変数の初期化
			g_Parts[i].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_Parts[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
			g_Parts[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

			// パーツの読み込みはまだしていない
			g_Parts[i].load = 0;
		}


		if (g_CameraMode == THIRD_PERSON)
		{
			playtpsanimation();
		}
		/*else if (g_CameraMode == FIRST_PERSON)
		{
			playfpsanimation();
		}*/
	

	
	return S_OK;
}

void playtpsanimation(void)
{
		g_Parts[0].use = TRUE;
		g_Parts[0].parent = &g_Player;	// 親をセット
		g_Parts[0].tblNo = 0;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[0].load = 1;
		LoadModel(MODEL_PLAYER_HEAD, &g_Parts[0].model);

		g_Parts[1].use = TRUE;
		g_Parts[1].parent = &g_Player;	// 親をセット
		g_Parts[1].tblNo = 1;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[1].tblMax = sizeof(move_tbl_larm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[1].load = 1;
		LoadModel(MODEL_PLAYER_LARM, &g_Parts[1].model);

		g_Parts[2].use = TRUE;
			g_Parts[2].parent = &g_Player;	// 親をセット
		g_Parts[2].tblNo = 2;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[2].tblMax = sizeof(move_tbl_rarm) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[2].load = 1;
		LoadModel(MODEL_PLAYER_RARM, &g_Parts[2].model);

		g_Parts[3].use = TRUE;
		g_Parts[3].parent = &g_Player;	// 親をセット
		g_Parts[3].tblNo = 3;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[3].tblMax = sizeof(move_tbl_lleg) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[3].load = 1;
		LoadModel(MODEL_PLAYER_LLEG, &g_Parts[3].model);

		g_Parts[4].use = TRUE;
		g_Parts[4].parent = &g_Player;	// 親をセット
		g_Parts[4].tblNo = 4;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[4].tblMax = sizeof(move_tbl_rleg) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[4].load = 1;
		LoadModel(MODEL_PLAYER_RLEG, &g_Parts[4].model);
}







//void playfpsanimation(void)
//{
//	g_Parts[1].use = TRUE;
//	g_Parts[1].parent = &g_Player;	// 親をセット
//	g_Parts[1].tblNo = 1;			// 再生するアニメデータの先頭アドレスをセット
//	g_Parts[1].tblMax = sizeof(Fmove_tbl_larm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
//	g_Parts[1].load = 1;
//	LoadModel(MODEL_PLAYER_LARM, &g_Parts[1].model);
//
//	g_Parts[2].use = TRUE;
//	g_Parts[2].parent = &g_Player;	// 親をセット
//	g_Parts[2].tblNo = 2;			// 再生するアニメデータの先頭アドレスをセット
//	g_Parts[2].tblMax = sizeof(Fmove_tbl_rarm) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
//	g_Parts[2].load = 1;
//	LoadModel(MODEL_PLAYER_RARM, &g_Parts[2].model);
//
//
//
//}

void UpdateAttackAnimation(void)
{
	if (g_Parts[2].use == TRUE)
	{
		if (g_Parts[2].tblMax == 0) // Animasyon bitti mi?
		{
			g_Parts[2].use = FALSE; // Animasyonu durdur
		}
	}
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}


	// パーツの解放処理
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{

	CAMERA* cam = GetCamera();
	g_Player.move = FALSE;
	INTERPOLATION_DATA** currentTblAdr;
	g_Player.attack = FALSE;

	// Update camera mode based on input
	if (GetKeyboardTrigger(DIK_X))
	{
		g_CameraMode = FIRST_PERSON;
	}
	if (GetKeyboardTrigger(DIK_C))
	{
		g_CameraMode = THIRD_PERSON;
	}



	// Movement logic
	if (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_A))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI / 2;
		g_Player.move = TRUE;
	}
	if (GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_D))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = -XM_PI / 2;
		g_Player.move = TRUE;
	}
	if (GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_W))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI;
		g_Player.move = TRUE;
	}
	if (GetKeyboardPress(DIK_DOWN) || GetKeyboardPress(DIK_S))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = 0.0f;
		g_Player.move = TRUE;
	}

	// Diagonal movement logic
	if (GetKeyboardPress(DIK_A) && GetKeyboardPress(DIK_W))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = 3 * XM_PI / 4;
		g_Player.move = TRUE;
	}
	if (GetKeyboardPress(DIK_A) && GetKeyboardPress(DIK_S))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI / 4;
		g_Player.move = TRUE;
	}
	if (GetKeyboardPress(DIK_D) && GetKeyboardPress(DIK_W))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = 5 * XM_PI / 4;
		g_Player.move = TRUE;
	}
	if (GetKeyboardPress(DIK_D) && GetKeyboardPress(DIK_S))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = 7 * XM_PI / 4;
		g_Player.move = TRUE;
	}

	



	currentTblAdr = (g_Player.move) ? g_MoveTblAdr : g_IdleTblAdr;
	/*currentTblAdr = (g_Player.move) ? g_FMoveTblAdr : g_FIdleTblAdr;*/
		
	// レイキャストして足元の高さを求める
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	BOOL ans = RayHitField(g_Player.pos, &HitPosition, &Normal);
	if (ans)
	{
		g_Player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
	}
	else
	{
		g_Player.pos.y = PLAYER_OFFSET_Y;
		Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}


#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}
#endif

	if (g_Player.spd > 0.0f)
	{
		g_Player.rot.y = g_Player.dir + cam->rot.y;
		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}

	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);




	g_Player.spd *= 0.5f;

	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if ((g_Parts[i].use == TRUE) && (g_Parts[i].tblMax > 0))
		{
			int nowNo = (int)g_Parts[i].time;
			int maxNo = g_Parts[i].tblMax;
			int nextNo = (nowNo + 1) % maxNo;
			INTERPOLATION_DATA* tbl = currentTblAdr[g_Parts[i].tblNo];

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;

			float nowTime = g_Parts[i].time - nowNo;

			Pos *= nowTime;
			Rot *= nowTime;
			Scl *= nowTime;

			XMStoreFloat3(&g_Parts[i].pos, nowPos + Pos);
			XMStoreFloat3(&g_Parts[i].rot, nowRot + Rot);
			XMStoreFloat3(&g_Parts[i].scl, nowScl + Scl);
			
			g_Parts[i].time += 1.0f / tbl[nowNo].frame;
			if ((int)g_Parts[i].time >= maxNo)
			{
				g_Parts[i].time -= maxNo;
			}

			
		}
	}

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;


	g_Player.UpVector = Normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.UpVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	//nvx = vx / len;
	angle = asinf(len);

	//quat = XMQuaternionIdentity();

//	quat = XMQuaternionRotationAxis(nvx, angle);
	quat = XMQuaternionRotationNormal(nvx, angle);


	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.Quaternion), quat, 0.05f);
	XMStoreFloat4(&g_Player.Quaternion, quat);

#ifdef _DEBUG
	PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	if (g_CameraMode == THIRD_PERSON)
	{
		DrawTPSPlayer();
	}
	else
	{
		/*DrawFPSPlayer();*/
	}

	SetCullingMode(CULL_MODE_BACK);
}




void DrawTPSPlayer(void)
{
	if (g_Player.use == FALSE) return;

	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	CAMERA* g_Camera = GetCamera();

	// 🎯 **Kameranın ileri yönünü hesapla**
	XMVECTOR cameraForward = XMVectorSubtract(XMLoadFloat3(&g_Camera->Target), XMLoadFloat3(&g_Camera->Eye));
	cameraForward = XMVector3Normalize(cameraForward);

	// 🎯 **Y ekseni dönüş açısını hesapla (atan2 ile doğru hesaplama)**
	float playerRotationY = atan2f(-XMVectorGetX(cameraForward), -XMVectorGetZ(cameraForward));

	// 🎯 **Karakterin rotasyonunu güncelle**
	g_Player.rot.x = playerRotationY;
	g_Player.rot.z = playerRotationY;

	// 🎯 **Dünya matrisi hesaplama (Scaling → Rotation → Translation)**
	mtxWorld = XMMatrixIdentity();
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y, g_Player.rot.z);
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);

	// 🎯 **Matrisleri doğru sırayla uygula**
	mtxWorld = XMMatrixMultiply(mtxScl, mtxRot);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// 🎯 **Matrisleri kaydet ve modeli çiz**
	SetWorldMatrix(&mtxWorld);
	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);
	DrawModel(&g_Player.model);

	// 🎯 **Karakterin parçalarını çiz**
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		mtxWorld = XMMatrixIdentity();
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);


		mtxWorld = XMMatrixMultiply(mtxScl, mtxRot);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);
		if (g_Parts[i].use == FALSE) continue;

		SetWorldMatrix(&mtxWorld);
		DrawModel(&g_Parts[i].model);
	}

}



// player fps mode code -- workıng
//void DrawFPSPlayer(void)
//{
//	CAMERA* g_Camera = GetCamera();
//	if (g_Player.use == FALSE) return;
//
//	SetCullingMode(CULL_MODE_NONE);
//	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;
//
//	// Oyuncunun Dünya Matrisi
//	mtxWorld = XMMatrixIdentity();
//	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
//	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
//	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
//
//	SetWorldMatrix(&mtxWorld);
//	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);
//	DrawModel(&g_Player.model);
//
//	// **Ellerin Kamera Pozisyonuna Sabitlenmesi**
//	XMVECTOR camPos = XMLoadFloat3(&g_Camera->pos);
//	XMMATRIX camView = XMLoadFloat4x4(&g_Camera->mtxView);
//	XMMATRIX camInv = XMMatrixInverse(nullptr, camView); // Kameranın ters matrisi
//
//	// Kameraya göre el pozisyonunu belirleme (kamera biraz önüne, aşağı ve sağa koy)
//	XMVECTOR handOffset = XMVectorSet(0.2f, -0.1f, 0.3f, 0.0f);
//	XMVECTOR handPos = XMVectorAdd(camPos, XMVector3Transform(handOffset, camInv));
//
//	// Ellerin dünya matrisi
//	XMMATRIX mtxHandWorld = XMMatrixIdentity();
//	mtxHandWorld *= XMMatrixScaling(1.0f, 1.0f, 1.0f); // Ölçek
//	mtxHandWorld *= camInv;  // Kameraya hizala (düzgün yönlendirme için inversini alıyoruz)
//	mtxHandWorld *= XMMatrixTranslationFromVector(handPos); // Pozisyonu uygula
//
//	// Matris ayarla ve elleri çiz
//	SetWorldMatrix(&mtxHandWorld);
//
//	for (int i = 1; i <= 2; i++)  // 1 ve 2 numaralı parçaları (kollar) kamera ile hareket ettir
//	{
//		if (g_Parts[i].use == FALSE) continue;
//
//		// Eller kameranın önüne sabit
//		XMMATRIX mtxPartWorld = mtxHandWorld;
//		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxPartWorld);
//
//		SetWorldMatrix(&mtxPartWorld);
//		DrawModel(&g_Parts[i].model);
//	}
//}




//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

