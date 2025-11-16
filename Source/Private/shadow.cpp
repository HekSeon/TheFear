//=============================================================================
//
// 影処理 [shadow.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "shadow.h"
#include "light.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	SHADOW_SIZE_X		(50.0f)			// 頂点サイズ
#define	SHADOW_SIZE_Z		(50.0f)			// 頂点サイズ

#define	MAX_SHADOW			(1024)			// 影最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;		// 位置
	XMFLOAT3	rot;		// 回転
	XMFLOAT3	scl;		// スケール
	MATERIAL	material;	// マテリアル
	BOOL		use;		// 使用しているかどうか

} SHADOW;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexShadow(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点情報
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static SHADOW					g_aShadow[MAX_SHADOW];		// 影ワーク
static int						g_TexNo;					// テクスチャ番号

static char* g_TextureName[] = {
	"data/TEXTURE/shadow000.jpg",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitShadow(void)
{
	// 頂点バッファの作成
	MakeVertexShadow();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// 影ワーク初期化
	for(int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		ZeroMemory(&g_aShadow[nCntShadow].material, sizeof(g_aShadow[nCntShadow].material));
		g_aShadow[nCntShadow].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aShadow[nCntShadow].pos = XMFLOAT3(0.0f, 0.1f, 0.0f);
		g_aShadow[nCntShadow].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aShadow[nCntShadow].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aShadow[nCntShadow].use = FALSE;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitShadow(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateShadow(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawShadow(void)
{
	// 減算合成
	SetBlendState(BLEND_MODE_SUBTRACT);

	// Z比較なし
	SetDepthEnable(FALSE);

	// フォグ無効
	SetFogEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for(int i = 0; i < MAX_SHADOW; i++)
	{
		if(g_aShadow[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_aShadow[i].scl.x, g_aShadow[i].scl.y, g_aShadow[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_aShadow[i].rot.x, g_aShadow[i].rot.y, g_aShadow[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_aShadow[i].pos.x, g_aShadow[i].pos.y, g_aShadow[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアルの設定
			SetMaterial(g_aShadow[i].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// 通常ブレンド
	SetBlendState(BLEND_MODE_ALPHABLEND);
	
	// Z比較あり
	SetDepthEnable(TRUE);

	// フォグを元に戻す
	SetFogEnable(GetFogEnable());
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexShadow()
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-SHADOW_SIZE_X / 2, 0.0f, SHADOW_SIZE_Z / 2);
		vertex[1].Position = XMFLOAT3(SHADOW_SIZE_X / 2, 0.0f, SHADOW_SIZE_Z / 2);
		vertex[2].Position = XMFLOAT3(-SHADOW_SIZE_X / 2, 0.0f, -SHADOW_SIZE_Z / 2);
		vertex[3].Position = XMFLOAT3(SHADOW_SIZE_X / 2, 0.0f, -SHADOW_SIZE_Z / 2);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

		// 拡散光の設定
		vertex[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[1].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[2].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[3].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
		vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
		vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
		vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// 影の作成
//=============================================================================
int CreateShadow(XMFLOAT3 pos, float fSizeX, float fSizeZ)
{
	int nIdxShadow = -1;

	for(int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		// 未使用（FALSE）だったらそれを使う
		if(!g_aShadow[nCntShadow].use)
		{
			g_aShadow[nCntShadow].pos = pos;
			g_aShadow[nCntShadow].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_aShadow[nCntShadow].scl = XMFLOAT3(fSizeX, 1.0f, fSizeZ);
			g_aShadow[nCntShadow].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_aShadow[nCntShadow].use = TRUE;

			nIdxShadow = nCntShadow;	// これがIndex番号
			break;
		}
	}

	return nIdxShadow;					// Index番号を返している
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorShadow(int nIdxShadow, XMFLOAT4 col)
{
	if (nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		g_aShadow[nIdxShadow].material.Diffuse = col;
	}
}

//=============================================================================
// 影の破棄
//=============================================================================
void ReleaseShadow(int nIdxShadow)
{
	if(nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		g_aShadow[nIdxShadow].use = FALSE;
	}
}

//=============================================================================
// 位置の設定
//=============================================================================
void SetPositionShadow(int nIdxShadow, XMFLOAT3 pos)
{
	if (nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		g_aShadow[nIdxShadow].pos = pos;
	}
}

