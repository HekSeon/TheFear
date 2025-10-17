/////////////////////////////////////////////////////////////////////////
//
// skydome.cpp
// 
// Author: hekSeon
// 
////////////////////////////////////////////////////////////////////////

#include "main.h"

#include "game.h"
#include "skydome.h"
#include "light.h"
#include "debugproc.h"

#define SKYDOME_MODEL "data/MODEL/skydome.obj"

static SKYDOME g_Skydome;

int g_skydome_load = 0;

//=============================================================================
// 初期化処理 (Skydome'u başlatma)
//=============================================================================
HRESULT InitSkydome(void)
{
	
		LoadModel(SKYDOME_MODEL, &g_Skydome.model);

		for (unsigned short i = 0; i < g_Skydome.model.SubsetNum; i++)
		{
			SetMaterial(g_Skydome.model.SubsetArray[i].Material.Material);
		}
		g_Skydome.load = TRUE;

		g_Skydome.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Skydome.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Skydome.scl = XMFLOAT3(500.0f, 500.0f, 500.0f);

		g_Skydome.spd = 0.0f;			// 移動スピードクリア

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Skydome.model, &g_Skydome.diffuse[0]);


		g_Skydome.use = TRUE;			// TRUE:生きてる



	return S_OK;
}

//=============================================================================
// 終了処理 (Skydome'u temizleme)
//=============================================================================
void UninitSkydome(void)
{
    if (g_Skydome.load)
    {
        UnloadModel(&g_Skydome.model);
        g_Skydome.load = FALSE;

    }
}

//=============================================================================
// 更新処理 (Güncelleme işlemi, Skydome sabit olduğu için genelde boş olur)
//=============================================================================
void UpdateSkydome(void)
{
		/*LIGHT sky;
		sky.Direction = XMFLOAT3(0.0f, 50.0f, 0.0f);
		sky.Diffuse = XMFLOAT4(0.4f, 0.5f, 0.7f, 1.0f);
		sky.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f,1.0f);
		sky.Type = LIGHT_TYPE_DIRECTIONAL;
		sky.Enable = TRUE;
		SetLight(1, &sky);*/
 
}

//=============================================================================
// 描画処理 (Skydome'u ekrana çizme)
//=============================================================================
void DrawSkydome(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	SetCullingMode(CULL_MODE_BACK);

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Skydome.scl.x, g_Skydome.scl.y, g_Skydome.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Skydome.rot.x, g_Skydome.rot.y, g_Skydome.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Skydome.pos.x, g_Skydome.pos.y, g_Skydome.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Skydome.mtxWorld, mtxWorld);



	// モデル描画
	DrawModel(&g_Skydome.model);


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);




}

SKYDOME *GetSkydome(void)
{
    return &g_Skydome;
}
