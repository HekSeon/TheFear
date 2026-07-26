//=============================================================================
//
// 木処理 [tree.cpp]
// Author : BAYAR "HekSeon" SEMIH
//
//=============================================================================
#include <random>

#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "tree.h"
#include "model.h"
#include "collision.h"
#include "player.h"
#include "meshfield.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(5)				// テクスチャの数

#define	TREE_WIDTH			(50.0f)			// 頂点サイズ
#define	TREE_HEIGHT			(80.0f)			// 頂点サイズ

#define	MAX_TREE			(1024)			// 木最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			nIdxShadow;		// 影ID
	BOOL		load;
	BOOL		use;			// 使用しているかどうか
	DX11_MODEL			model;
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];

} TREE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexTree(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
#define MODEL_TREE "data/MODEL/tree.obj"
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static TREE					g_aTree[MAX_TREE];	// 木ワーク
static int					g_TexNo;			// テクスチャ番号
static BOOL					g_bAlpaTest;		// アルファテストON/OFF
//static int					g_nAlpha;			// アルファテストの閾値

static char *g_TextureName[] =
{
	"data/MODEL/tree.obj",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTree(void)
{
	// 1. Önce tüm ağaç havuzunu ve modellerini bir kez yükleyip hazırlayalım
	for (int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		LoadModel(MODEL_TREE, &g_aTree[nCntTree].model);
		g_aTree[nCntTree].load = TRUE;
		g_aTree[nCntTree].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_aTree[nCntTree].scl = XMFLOAT3(0.3f, 0.3f, 0.3f); // Ağaç ölçeği
		g_aTree[nCntTree].use = FALSE; // Başlangıçta pasif
		GetModelDiffuse(&g_aTree[nCntTree].model, &g_aTree[nCntTree].diffuse[0]);
	}

	// 木の設定
	const float worldSize = 4000.0f;          // Dünya sınırı (-500 ile +500)
	const int maxTrees = 350;                // Yerleştirilecek ağaç sayısı
	const float minDistance = 130.0f;
	const int maxAttempts = 150;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(-worldSize, worldSize);

	int placed = 0;
	while (placed < maxTrees)
	{
		bool valid = false;
		XMFLOAT3 candidatePos;

		for (int attempt = 0; attempt < maxAttempts; attempt++)
		{
			candidatePos = XMFLOAT3(dist(gen), 0.0f, dist(gen));

			XMFLOAT3 hitPos, normal;
			if (RayHitField(candidatePos, &hitPos, &normal))
			{
				candidatePos.y = hitPos.y;
			}
			else
			{
				candidatePos.y = 0.0f;
			}

			bool tooClose = false;
			for (int i = 0; i < placed; i++)
			{
				float dx = candidatePos.x - g_aTree[i].pos.x;
				float dz = candidatePos.z - g_aTree[i].pos.z;
				float distSq = dx * dx + dz * dz;

				if (distSq < minDistance * minDistance)
				{
					tooClose = true;
					break;
				}
			}

			if (!tooClose)
			{
				valid = true;
				break;
			}
		}

		if (valid)
		{
			SetTree(candidatePos, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			placed++;
		}
		else
		{
			break;
		}
	}

	g_bAlpaTest = TRUE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTree(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTree(void)
{

	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		if(g_aTree[nCntTree].use)
		{
			// 影の位置設定
			SetPositionShadow(g_aTree[nCntTree].nIdxShadow, XMFLOAT3(g_aTree[nCntTree].pos.x, 0.01f, g_aTree[nCntTree].pos.z));

			PLAYER* g_player = GetPlayer();
			g_player->oldpos = g_player->pos;
			
			if (CollisionBC(g_player->pos, g_aTree[nCntTree].pos, 15.0f, 15.0f))
			{
				g_player->pos = g_player->oldpos;
				break;
			}
		}
	}


#ifdef _DEBUG
	// アルファテストON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE: TRUE;
	}

	

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTree(void)
{
	// αテスト設定
	if (g_bAlpaTest == TRUE)
	{
		SetAlphaTestEnable(TRUE);
	}

	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// DİKKAT: Burada eskiden kalan g_VertexBuffer (NULL) ataması model çizimini bozar, sildik.

	// プリミティブトポロジ設定 (Model çiziminde genellikle gerekmez ama kalabilir)
	// GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_TREE; i++)
	{
		if (g_aTree[i].use)
		{
			mtxWorld = XMMatrixIdentity();
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			mtxScl = XMMatrixScaling(g_aTree[i].scl.x, g_aTree[i].scl.y, g_aTree[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			mtxTranslate = XMMatrixTranslation(g_aTree[i].pos.x, g_aTree[i].pos.y, g_aTree[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			SetWorldMatrix(&mtxWorld);

			DrawModel(&g_aTree[i].model);
		}
	}

	SetFogEnable(TRUE);
	SetLightEnable(TRUE);
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexTree(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 木のパラメータをセット
//=============================================================================
int SetTree(const XMFLOAT3& pos, const XMFLOAT4& color)
{
	for (int i = 0; i < MAX_TREE; i++)
	{
		if (!g_aTree[i].use)
		{
			g_aTree[i].pos = pos;
			g_aTree[i].material.Diffuse = color;
			g_aTree[i].use = TRUE;
			// g_aTree[i].nIdxShadow = CreateShadow(pos, 1.0f, 1.0f); // Gölge havuzu taşmasını önlemek için geçici kapatın
			return i;
		}
	}
	return -1;
}