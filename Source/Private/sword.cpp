//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "sword.h"
#include "shadow.h"
#include "light.h"
#include "player.h"
#include "collision.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	MODEL_PLAYER		"data/MODEL/player.obj"			// 読み込むモデル名

#define	MODEL_SWORD		"data/MODEL/sword.obj"	// 読み込むモデル名

#define MAX_MODEL_SWORD		(1)								// プレイヤーの数

PLAYER* player = GetPlayer();
enum SwordState {
	FOLLOW_PLAYER,
	ATTACK_ENEMY,
	RETURN_TO_PLAYER
};

SwordState g_SwordState = FOLLOW_PLAYER;

XMFLOAT3 g_TargetPos; // Kılıcın gitmesi gereken hedef pozisyonu

float g_SwordSpeed = 1.2f; // Kılıcın hareket hızı


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static SWORD	g_Sword;						// プレイヤー

static BOOL g_Load = FALSE;

static float speed = 1.0f;

// プレイヤーの階層アニメーションデータ
// プレイヤーの左右パーツを動かしているアニメデータ



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSword(void)
{
	LoadModel(MODEL_SWORD, &g_Sword.model);
	g_Sword.load = TRUE;

	PLAYER* player = GetPlayer();



	g_Sword.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Sword.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Sword.scl = XMFLOAT3(0.1f, 0.1f, 0.1f);

	g_Sword.size = SWORD_SIZE;	// 当たり判定の大きさ

	g_Sword.use = TRUE;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Sword.pos;
	pos.y -= (0.0f - 0.1f);
	g_Sword.shadowIdx = CreateShadow(pos, 1.0f, 1.0f);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号




		// 位置・回転・スケールの初期設定
		g_Sword.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sword.rot = XMFLOAT3(180.0f, 0.0f, 0.0f);
		g_Sword.scl = XMFLOAT3(0.1f, 0.1f, 0.1f);

	

			// 階層アニメーション用のメンバー変数の初期化
		g_Sword.time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Sword.tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Sword.tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Sword.load = 0;


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSword(void)
{
	// モデルの解放処理
	if (g_Sword.load)
	{
		UnloadModel(&g_Sword.model);
		g_Sword.load = FALSE;
	}


	// パーツの解放処理
	for (int i = 0; i < MAX_MODEL_SWORD; i++)
	{
		if (g_Sword.load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_Sword.model);
			g_Sword.load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSword(void)
{
#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Sword.pos.z = g_Sword.pos.x = 0.0f;
		g_Sword.rot.y = g_Sword.dir = 0.0f;
	}
#endif


	if (g_SwordState == FOLLOW_PLAYER)
	{
		
		// 🎯 Kılıcı oyuncunun etrafında belirli bir mesafede tut
		XMFLOAT3 followOffset = { 50.0f, 30.0f, -40.0f }; // Oyuncunun biraz üstünde
		XMMATRIX playerMatrix = XMLoadFloat4x4(&player->mtxWorld);
		XMVECTOR offsetVector = XMVectorSet(followOffset.x, followOffset.y, followOffset.z, 1.0f);
		offsetVector = XMVector3TransformCoord(offsetVector, playerMatrix);
		XMStoreFloat3(&g_Sword.pos, offsetVector);
		// Kılıcı oyuncunun dönüşüne ayarla ve Y ekseninde 180° çevir
		XMFLOAT3 swordRot;
		XMStoreFloat3(&swordRot, XMLoadFloat3(&player->rot));
		swordRot.y += XM_PI; // 180 derece döndürme (XM_PI = 3.14159)

		// Güncellenmiş rotasyonu ata
		XMStoreFloat3(&g_Sword.rot, XMLoadFloat3(&swordRot));
	}

	else if (g_SwordState == ATTACK_ENEMY)
	{
		// Kılıcın mevcut pozisyonunu yükle
		XMVECTOR swordPos = XMLoadFloat3(&g_Sword.pos);
		XMVECTOR targetPos = XMLoadFloat3(&g_TargetPos);

		// Hedefe doğru yönü hesapla
		XMVECTOR direction = XMVectorSubtract(targetPos, swordPos);
		if (XMVectorGetX(XMVector3Length(direction)) < 0.01f)
		{
			g_SwordState = FOLLOW_PLAYER; // Kılıç düşmana ulaşınca geri dön
			return;
		}
		direction = XMVector3Normalize(direction);

		// Kılıcı hedefe doğru hareket ettir
		float speed = 1.0f; // Hızı artır
		swordPos = XMVectorAdd(swordPos, XMVectorScale(direction, speed));
		XMStoreFloat3(&g_Sword.pos, swordPos);


		// Debug amaçlı kılıcın pozisyonunu ekrana yazdır (Opsiyonel)
		// printf("Sword Position: %.2f, %.2f, %.2f\n", g_Sword.pos.x, g_Sword.pos.y, g_Sword.pos.z);
	}

	else if (g_SwordState == RETURN_TO_PLAYER)
	{
		// 🎯 Kılıcı oyuncuya geri döndür
		XMVECTOR swordPos = XMLoadFloat3(&g_Sword.pos);
		XMVECTOR playerPos = XMLoadFloat3(&player->pos);
		XMVECTOR direction = XMVector3Normalize(XMVectorSubtract(playerPos, swordPos));
		swordPos = XMVectorAdd(swordPos, XMVectorScale(direction, g_SwordSpeed));

		XMStoreFloat3(&g_Sword.pos, swordPos);

		// 🎯 Eğer oyuncuya ulaştıysa, tekrar takip moduna geç
		float distance = XMVectorGetX(XMVector3Length(XMVectorSubtract(playerPos, swordPos)));
		if (distance < 0.1f) {
			g_SwordState = FOLLOW_PLAYER;
		}

	}

	if (GetKeyboardTrigger(DIK_SPACE))
	{
		AttackNearestEnemy();
	}

	if (GetKeyboardTrigger(DIK_F))
	{
		if (g_SwordState == ATTACK_ENEMY)
		{
			g_SwordState = FOLLOW_PLAYER;
		}
	}

#ifdef _DEBUG
	PrintDebugProc("SwordState: %d \n", g_SwordState);
#endif // _DEBUG

}

	// 🎯 Kılıcı en yakın düşmana yönlendir
void AttackNearestEnemy()
{
	if (g_SwordState == FOLLOW_PLAYER)
	{
		g_SwordState = ATTACK_ENEMY;
		ENEMY* nearestEnemy = FindNearestEnemy(g_Sword.pos);

		if (nearestEnemy) {
			g_TargetPos = nearestEnemy->pos;
			g_SwordState = ATTACK_ENEMY;

			// Kılıcı düşmana doğru hareket ettir
			XMVECTOR swordPos = XMLoadFloat3(&g_Sword.pos);
			XMVECTOR targetPos = XMLoadFloat3(&g_TargetPos);
			XMVECTOR direction = XMVector3Normalize(XMVectorSubtract(targetPos, swordPos));

			// Kılıcı hedefe doğru hareket ettir
			swordPos = XMVectorAdd(swordPos, XMVectorScale(direction, speed)); // İleri hareket
			XMStoreFloat3(&g_Sword.pos, swordPos); // Yeni pozisyonu sakla

			ENEMY* enemies = GetEnemy();
			
			// Hasar ver
			if (CollisionBC(g_TargetPos, g_Sword.pos, 1.0f, 1.0f))
			{
				enemies->health -= 50;

				if (enemies->health <= 0)
				{
					enemies->isAlive = FALSE;
					enemies->use = FALSE;
				}
				g_SwordState = FOLLOW_PLAYER;
			}
		}
	}
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawSword(void)
{
	if (g_Sword.use == FALSE) return;

	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxPlayer;

	// 🎯 **Dünya matrisi hesaplama (Scaling → Rotation → Translation)**
	mtxPlayer = XMMatrixIdentity();
	mtxScl = XMMatrixScaling(g_Sword.scl.x, g_Sword.scl.y, g_Sword.scl.z);
	mtxRot = XMMatrixRotationRollPitchYaw(g_Sword.rot.x, g_Sword.rot.y, g_Sword.rot.z);
	mtxTranslate = XMMatrixTranslation(g_Sword.pos.x, g_Sword.pos.y, g_Sword.pos.z);

	// 🎯 **Matrisleri doğru sırayla uygula**
	mtxPlayer = XMMatrixMultiply(mtxScl, mtxRot);
	mtxPlayer = XMMatrixMultiply(mtxPlayer, mtxTranslate);

	// 🎯 **Matrisleri kaydet ve modeli çiz**
	SetWorldMatrix(&mtxPlayer);
	XMStoreFloat4x4(&g_Sword.mtxWorld, mtxPlayer);
	DrawModel(&g_Sword.model);
}




//=============================================================================
// プレイヤー情報を取得
//=============================================================================
SWORD* GetSword(void)
{
	return &g_Sword;
}

