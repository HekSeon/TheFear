//=============================================================================
//
// モデル処理 [player.cpp]
// Author : BAYAR "HekSeon" SEMIH
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
#include "fade.h"
#include "playeranimation.h"
#include "game.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	MODEL_PLAYER		"data/MODEL/player.obj"			// 読み込むモデル名





#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(16.065f)							// プレイヤーの足元をあわせる


	

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************




//*****************************************************************************
// グローバル変数
//*****************************************************************************

static PLAYER g_Player;   // プレイヤー

static int g_AttackFrame = 0;
static bool g_AttackRequested = false;
constexpr int AttackHitFrame = 16;
constexpr int AttackEndFrame = 30;
constexpr float AttackReach = 18.0f;
constexpr float AttackRadius = 12.0f;





//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	// Body modeli
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, 0.0f };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 0.5f, 0.5f, 0.5f };

	g_Player.spd = 0.0f;
	g_Player.size = PLAYER_SIZE;

	g_Player.use = TRUE;

	g_Player.health = 100.0f;
	g_AttackFrame = 0;
	g_AttackRequested = false;
	g_Player.attack = FALSE;
	XMFLOAT3 ground, normal;
	if (RayHitField(g_Player.pos, &ground, &normal))
		g_Player.pos.y = ground.y + PLAYER_OFFSET_Y;
	g_Player.oldpos = g_Player.pos;
	XMStoreFloat4x4(&g_Player.mtxWorld, XMMatrixIdentity());

	// gölge oluştur
	XMFLOAT3 shPos = g_Player.pos;
	shPos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(shPos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);

	// parent: gövde kendisi (NULL) — parçaların parent'ı body olacak
	g_Player.currentTblAdr = 0;

	// Parçaları başlat (head, larm, rarm, lleg, rleg)
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// transform / flags sıfırla
		g_Player.parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player.parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player.parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		XMStoreFloat4x4(&g_Player.parts[i].mtxWorld, XMMatrixIdentity());

		g_Player.parts[i].use = FALSE;
		g_Player.parts[i].load = FALSE;

		g_Player.parts[i].time = 0.0f;
		g_Player.parts[i].tblNo = 0;
		g_Player.parts[i].tblMax = 0;
		g_Player.parts[i].tbl = nullptr;

		g_Player.parts[i].parent = &g_Player;
	}

	// Body'yi de yükledik zaten; şimdi parçaların modellerini yükle ve yerleştir
	// Head
	LoadModel(MODEL_PLAYER_HEAD, &g_Player.parts[0].model);
	g_Player.parts[0].pos = XMFLOAT3(0.0f, 5.0f, 0.0f);
	g_Player.parts[0].rot = XMFLOAT3(0, 0, 0);
	g_Player.parts[0].scl = XMFLOAT3(1, 1, 1);
	g_Player.parts[0].use = TRUE;
	g_Player.parts[0].load = TRUE;

	// LArm
	LoadModel(MODEL_PLAYER_LARM, &g_Player.parts[1].model);
	g_Player.parts[1].pos = XMFLOAT3(-2.0f, 3.5f, 0.0f);
	g_Player.parts[1].use = TRUE;
	g_Player.parts[1].load = TRUE;

	// RArm
	LoadModel(MODEL_PLAYER_RARM, &g_Player.parts[2].model);
	g_Player.parts[2].pos = XMFLOAT3(2.0f, 3.5f, 0.0f);
	g_Player.parts[2].use = TRUE;
	g_Player.parts[2].load = TRUE;

	// LLeg
	LoadModel(MODEL_PLAYER_LLEG, &g_Player.parts[3].model);
	g_Player.parts[3].pos = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	g_Player.parts[3].use = TRUE;
	g_Player.parts[3].load = TRUE;

	// RLeg
	LoadModel(MODEL_PLAYER_RLEG, &g_Player.parts[4].model);
	g_Player.parts[4].pos = XMFLOAT3(1.0f, 0.0f, 0.0f);
	g_Player.parts[4].use = TRUE;
	g_Player.parts[4].load = TRUE;

	// Animasyon sistemi başlangıç
	InitPlayerAnimation(&g_Player);
	SetPlayerAnimation(&g_Player, ANIM_IDLE);

	return S_OK;
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
		if (g_Player.parts[i].load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_Player.parts[i].model);
			g_Player.parts[i].load = FALSE;
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
	if (!g_Player.use) return;
	if (g_Player.health <= 0.0f)
	{
		g_Player.use = FALSE;
		SetFade(FADE_OUT, MODE_RESULT);
		return;
	}

	// Kamera mod değişimi
	if (GetKeyboardTrigger(DIK_X)) g_CameraMode = FIRST_PERSON;
	if (GetKeyboardTrigger(DIK_C)) g_CameraMode = THIRD_PERSON;

	// Hareket kontrolü (sadece pozisyon)
	g_Player.spd = 0.0f;
	if (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_A)) { g_Player.spd = VALUE_MOVE; g_Player.dir = XM_PI / 2; g_Player.move = TRUE; }
	if (GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_D)) { g_Player.spd = VALUE_MOVE; g_Player.dir = -XM_PI / 2; g_Player.move = TRUE; }
	if (GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_W)) { g_Player.spd = VALUE_MOVE; g_Player.dir = XM_PI; g_Player.move = TRUE; }
	if (GetKeyboardPress(DIK_DOWN) || GetKeyboardPress(DIK_S)) { g_Player.spd = VALUE_MOVE; g_Player.dir = 0.0f; g_Player.move = TRUE; }

	// Diagonal hareket
	if (GetKeyboardPress(DIK_A) && GetKeyboardPress(DIK_W)) { g_Player.spd = VALUE_MOVE; g_Player.dir = 3 * XM_PI / 4; g_Player.move = TRUE; }
	if (GetKeyboardPress(DIK_A) && GetKeyboardPress(DIK_S)) { g_Player.spd = VALUE_MOVE; g_Player.dir = XM_PI / 4; g_Player.move = TRUE; }
	if (GetKeyboardPress(DIK_D) && GetKeyboardPress(DIK_W)) { g_Player.spd = VALUE_MOVE; g_Player.dir = 5 * XM_PI / 4; g_Player.move = TRUE; }
	if (GetKeyboardPress(DIK_D) && GetKeyboardPress(DIK_S)) { g_Player.spd = VALUE_MOVE; g_Player.dir = 7 * XM_PI / 4; g_Player.move = TRUE; }


	const bool startAttack = g_AttackRequested && g_AttackFrame == 0 &&
		IsAppFocused() && !IsGameDebugOpen();
	g_AttackRequested = false;
	if (startAttack)
	{
		g_AttackFrame = 1;
		g_Player.rot.y = XM_PI + cam->rot.y;
	}
	else if (g_AttackFrame > 0)
	{
		if (++g_AttackFrame > AttackEndFrame)
			g_AttackFrame = 0;
	}
	g_Player.attack = g_AttackFrame != 0;

	g_Player.oldpos = g_Player.pos;
	XMFLOAT3 next = g_Player.pos;
	if (g_Player.spd > 0.0f)
	{
		const float moveYaw = g_Player.dir + cam->rot.y;
		if (!g_Player.attack)
			g_Player.rot.y = moveYaw;

		const float speed = g_Player.attack ? g_Player.spd * 0.45f : g_Player.spd;
		next.x -= sinf(moveYaw) * speed;
		next.z -= cosf(moveYaw) * speed;
	}

	XMFLOAT3 ground, normal;
	if (RayHitField(next, &ground, &normal))
		g_Player.pos = {next.x, ground.y + PLAYER_OFFSET_Y, next.z};

	if (g_AttackFrame == AttackHitFrame)
		DamageEnemiesInRange(GetPlayerAttackCenter(), AttackRadius, 50.0f);

	XMFLOAT3 shadowPos = g_Player.pos;
	shadowPos.y -= PLAYER_OFFSET_Y - 0.1f;
	SetPositionShadow(g_Player.shadowIdx, shadowPos);

	SetPlayerAnimation(&g_Player, g_Player.attack ? ANIM_ATTACK :
		(g_Player.move ? ANIM_WALK : ANIM_IDLE));
	UpdatePlayerAnimation(&g_Player);
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

	mtxWorld = XMMatrixIdentity();
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxRot = XMMatrixRotationY(g_Player.rot.y + XM_PI);
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);

	mtxWorld = XMMatrixMultiply(mtxScl, mtxRot);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	SetWorldMatrix(&mtxWorld);
	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);
	DrawModel(&g_Player.model);

	const XMFLOAT3 pivots[] = {{0,-2,0}, {11,-4,0}, {-11,-4,0},
		{5,-23.5f,0}, {-5,-23.5f,0}};
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		const XMFLOAT3& pivot = pivots[i];
		mtxScl = XMMatrixScaling(g_Player.parts[i].scl.x, g_Player.parts[i].scl.y, g_Player.parts[i].scl.z);
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player.parts[i].rot.x, g_Player.parts[i].rot.y, g_Player.parts[i].rot.z);
		mtxTranslate = XMMatrixTranslation(g_Player.parts[i].pos.x, g_Player.parts[i].pos.y, g_Player.parts[i].pos.z);


		mtxWorld = XMMatrixTranslation(-pivot.x, -pivot.y, -pivot.z) *
			mtxScl * mtxRot * XMMatrixTranslation(pivot.x, pivot.y, pivot.z) * mtxTranslate;

		if (g_Player.parts[i].parent != NULL)
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Player.parts[i].parent->mtxWorld));
		}

		XMStoreFloat4x4(&g_Player.parts[i].mtxWorld, mtxWorld);
		if (g_Player.parts[i].use == FALSE) continue;

		SetWorldMatrix(&mtxWorld);
		DrawModel(&g_Player.parts[i].model);
	}

}

//=============================================================================
// プレイヤー情報を取得
//=============================================================================
void RequestPlayerAttack(void) { g_AttackRequested = true; }

XMFLOAT3 GetPlayerAttackCenter(void)
{
	return {g_Player.pos.x - sinf(g_Player.rot.y) * AttackReach, g_Player.pos.y,
		g_Player.pos.z - cosf(g_Player.rot.y) * AttackReach};
}

float GetPlayerAttackRadius(void) { return AttackRadius; }
bool IsPlayerAttackActive(void) { return g_AttackFrame == AttackHitFrame; }

PLAYER *GetPlayer(void)
{
	return &g_Player;
}

