//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"

#include "game.h"
#include "player.h"
#include "sword.h"
#include "enemy.h"
#include "skydome.h"
#include "meshfield.h"

#include "debugproc.h"

#include "score.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//void CheckHit(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 200, 200, 130.0f, 130.0f);

	InitSkydome();

	InitPlayer();
	
	InitSword();

	InitEnemy();

	InitScore();



	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	UninitScore();

	UninitEnemy();

	UninitSword();
	//UninitParticle();
	UninitPlayer();

	UninitMeshField();

	UninitSkydome();

	
	

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
	UpdateMeshField();

	UpdateSkydome();

	UpdatePlayer();

	UpdateSword();

	UpdateEnemy();

	


#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}

#endif

	if (g_bPause == FALSE)
		return;



	//CheckHit();

	
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	DrawMeshField();

	DrawSkydome();

	PLAYER* player = GetPlayer();
	XMFLOAT3 pos = player->pos;
	SetCameraAT(pos, -50, 20, 50);
	SetCamera();
	


	DrawPlayer();

	// 🎯 **Kılıcı sağ ele bağla**
	

	DrawSword(); // Kılıcı çiz

	DrawEnemy();

	
	SetDepthEnable(FALSE);

	
	SetLightEnable(FALSE);



//3d yapar
	SetLightEnable(TRUE);

	SetDepthEnable(TRUE);

	SetFogEnable(FALSE);
}


void DrawGame(void)
{
#ifdef _DEBUG
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif


	switch (g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);

		DrawGame0();
	
	
	
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// 当たり判定処理
//=============================================================================
//void CheckHit(void)
//{
//	ENEMY* enemy = GetEnemy();		// エネミーのポインターを初期化
//	PLAYER* player = GetPlayer();	// プレイヤーのポインターを初期化
//	BULLET* bullet = GetBullet();	// 弾のポインターを初期化
//
//	// 敵とプレイヤーキャラ
//	for (int i = 0; i < MAX_ENEMY; i++)
//	{
//		//敵の有効フラグをチェックする
//		if (enemy[i].use == FALSE)
//			continue;
//
//		//BCの当たり判定
//		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
//		{
//			// 敵キャラクターは倒される
//			enemy[i].use = FALSE;
//			ReleaseShadow(enemy[i].shadowIdx);
//
//			// スコアを足す
//			AddScore(100);
//		}
//	}
//
//
//	// プレイヤーの弾と敵
//	for (int i = 0; i < MAX_BULLET; i++)
//	{
//		//弾の有効フラグをチェックする
//		if (bullet[i].use == FALSE)
//			continue;
//
//		// 敵と当たってるか調べる
//		for (int j = 0; j < MAX_ENEMY; j++)
//		{
//			//敵の有効フラグをチェックする
//			if (enemy[j].use == FALSE)
//				continue;
//
//			//BCの当たり判定
//			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
//			{
//				// 当たったから未使用に戻す
//				bullet[i].use = FALSE;
//				ReleaseShadow(bullet[i].shadowIdx);
//
//				// 敵キャラクターは倒される
//				enemy[j].use = FALSE;
//				ReleaseShadow(enemy[j].shadowIdx);
//
//				// スコアを足す
//				AddScore(10);
//			}
//		}
//
//	}
//
//
//	// エネミーが全部死亡したら状態遷移
//	int enemy_count = 0;
//	for (int i = 0; i < MAX_ENEMY; i++)
//	{
//		if (enemy[i].use == FALSE) continue;
//		enemy_count++;
//	}
//
//	// エネミーが０匹？
//	if (enemy_count == 0)
//	{
//		SetFade(FADE_OUT, MODE_RESULT);
//	}
//
//}


