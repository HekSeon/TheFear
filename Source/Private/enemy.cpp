//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "shadow.h"
#include "game.h"
#include "fade.h"
#include "sound.h"
#include "player.h"
#include "collision.h"
#include "sword.h"
#include "score.h"
//#include "particle.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/enemy/bear.obj"		// 読み込むモデル名


#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(7.0f)						// エネミーの足元をあわせる

				// エネミーの数
#define INITIAL_ENEMIES 20    // Başlangıç düşman sayısı
#define SPAWN_INTERVAL 600   // 20 saniye (60 FPS × 20) => Frame sayacı kullanıyoruz

#define ENEMY_PUSHBACK_STRENGTH (0.2f)



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemies[MAX_ENEMY];				// エネミー

static BOOL				g_Enemies_load = FALSE;

int currentEnemyCount = 0;    // Şu anki düşman sayısı
int spawnTimer = 0;           // Zamanlayıcı (frame cinsinden)

bool collisionDetected = false;

// Hız (düşman saniyede kaç birim ilerlesin?)
float speed = 0.8f;




//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemies[i].model);
		g_Enemies[i].load = TRUE;

		g_Enemies[i].pos = XMFLOAT3(rand() % 700 - 550, 15.0f, rand() % 700 - 550); // Rastgele konum
		g_Enemies[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemies[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);

		g_Enemies[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemies[i].size = ENEMY_SIZE;	// 当たり判定の大きさ
		g_Enemies[i].health = 100.0f;
		g_Enemies[i].isAlive = FALSE;	

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemies[i].model, &g_Enemies[i].diffuse[0]);

		XMFLOAT3 pos = g_Enemies[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemies[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);
		
		g_Enemies[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Enemies[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Enemies[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		g_Enemies[i].use = FALSE;			// TRUE:生きてる

		// 階層アニメーション用の初期化処理

	}
	
	SpawnEnemy();
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{

		if (g_Enemies[i].load)
		{
			UnloadModel(&g_Enemies[i].model);
			g_Enemies[i].load = FALSE;
			
		}
	}


}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	PLAYER* player = GetPlayer();

	for (int i = 0; i < MAX_ENEMY; i++)
	{

		if (CollisionBB(g_Enemies[i].pos, 1.f, 1.f, player->pos, 1.f, 1.f))
		{
			player->health -= 1; // Oyuncunun sağlığını azal
		}

	// Zamanlayıcıyı artır
	spawnTimer++;

	// 20 saniyede bir düşman ekle
	if (spawnTimer >= SPAWN_INTERVAL && currentEnemyCount < MAX_ENEMY)
	{
		SpawnEnemy();
	}

	// Düşmanları güncelle
	
	if (g_Enemies[i].use)
	{
		PLAYER* player = GetPlayer();
		XMFLOAT3 direction;
		direction.x = player->pos.x - g_Enemies[i].pos.x;
		direction.y = player->pos.y - g_Enemies[i].pos.y;
		direction.z = player->pos.z - g_Enemies[i].pos.z;
		float length = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

		if (length > 0.01f) // Bölme hatasını önlemek için
		{
			direction.x /= length;
			direction.y /= length;
			direction.z /= length;
		}

		if (g_Enemies[i].health <= 0.0f)
		{
			g_Enemies[i].isAlive = FALSE;
			g_Enemies[i].use = FALSE;
			currentEnemyCount--;
		}

		// Eski pozisyonu sakla
		XMFLOAT3 oldPos = g_Enemies[i].pos;

		// Yeni pozisyon hesapla
		XMFLOAT3 newPos;
		newPos.x = g_Enemies[i].pos.x + direction.x * speed;
		newPos.y = g_Enemies[i].pos.y + direction.y * speed;
		newPos.z = g_Enemies[i].pos.z + direction.z * speed;

		bool collisionDetected = false;

		// Diğer düşmanlarla çarpışmayı kontrol et
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			if (i != j && g_Enemies[j].use)
			{
				if (CollisionBC(newPos, g_Enemies[j].pos, 5.0f, 5.0f)) // Çarpışma kontrolü
				{
					collisionDetected = true;
					break; // Daha fazla kontrol etmeye gerek yok
				}
			}
		}

		
	

			// Eğer çarpışma algılandıysa eski pozisyona dön
			if (collisionDetected)
			{
				g_Enemies[i].pos = oldPos;
			}
			else
			{
				g_Enemies[i].pos = newPos;
			}

			// Eğer çarpışma algılandıysa eski pozisyona dön

			

			if (currentEnemyCount <= 0)
			{
				//SetFade(FADE_OUT, MODE_RESULT);
			}

			

			// Gölge pozisyonunu güncelle
			XMFLOAT3 pos = g_Enemies[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemies[i].shadowIdx, pos);
		}
	}



#ifdef _DEBUG

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (GetKeyboardTrigger(DIK_P))
		{
			// モデルの色を変更できるよ！半透明にもできるよ。
			for (int j = 0; j < g_Enemies[i].model.SubsetNum; j++)
			{
				SetModelDiffuse(&g_Enemies[i].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));
			}
		}

		if (GetKeyboardTrigger(DIK_L))
		{
			// モデルの色を元に戻している
			for (int j = 0; j < g_Enemies[i].model.SubsetNum; j++)
			{
				SetModelDiffuse(&g_Enemies[i].model, j, g_Enemies[i].diffuse[0]);
			}
		}
	}
#endif


}

void SpawnEnemy()
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (!g_Enemies[i].use)  // Kullanılmayan (boş) düşman slotu
		{
			g_Enemies[i].isAlive = TRUE;
			g_Enemies[i].use = TRUE;
			g_Enemies[i].health = 100.0f;
			g_Enemies[i].shadowIdx = CreateShadow(g_Enemies[i].pos, 1.0f, 1.0f);
			currentEnemyCount++;
			spawnTimer = 0; // Sayaç sıfırla
			break;  // Tek bir düşman oluşturup çık
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemies[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemies[i].scl.x, g_Enemies[i].scl.y, g_Enemies[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemies[i].rot.x, g_Enemies[i].rot.y + XM_PI, g_Enemies[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemies[i].pos.x, g_Enemies[i].pos.y, g_Enemies[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemies[i].mtxWorld, mtxWorld);

		

		// モデル描画
		DrawModel(&g_Enemies[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


ENEMY* FindNearestEnemy(XMFLOAT3 swordPos)
{
	

	ENEMY* closestEnemy = nullptr;
	float minDistance = 1000.0f;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (!g_Enemies[i].isAlive) continue; // Ölü düşmanları atla

		XMVECTOR enemyPos = XMLoadFloat3(&g_Enemies[i].pos);
		XMVECTOR swordPosition = XMLoadFloat3(&swordPos);

		float distance = XMVectorGetX(XMVector3Length(XMVectorSubtract(enemyPos, swordPosition)));

		if (distance < minDistance) // En küçük mesafeyi bul
		{
			minDistance = distance;
			closestEnemy = &g_Enemies[i];
		}
	}

#ifdef _DEBUG

	PrintDebugProc("Kilic pozisyonu: (%f, %f, %f)\n", swordPos.x, swordPos.y, swordPos.z);
	if (closestEnemy) {
		PrintDebugProc("En yakin dusman bulundu: (%f, %f, %f)\n", closestEnemy->pos.x, closestEnemy->pos.y, closestEnemy->pos.z);
	}
	else {
		PrintDebugProc("Hata: En yakin dusman bulunamadi!\n");
	}
#endif // _DEBUG

	return closestEnemy;

}





//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy(void)
{
	//for (int i = 0; i < MAX_ENEMY; i++)
	//{
	//	if (g_Enemies[i].isAlive)  // Eğer düşman aktifse
	//	{
	//		return &g_Enemies[i];
	//	}
	//}
	return &g_Enemies[0];
}


