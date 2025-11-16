//=============================================================================
//
// 木処理 [tree.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "tree.h"
#include "model.h"
#include "collision.h"
#include "player.h"
#include <random>


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(5)				// テクスチャの数

#define	TREE_WIDTH			(50.0f)			// 頂点サイズ
#define	TREE_HEIGHT			(80.0f)			// 頂点サイズ

#define	MAX_TREE			(256)			// 木最大数

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
	//MakeVertexTree();

	//// テクスチャ生成
	//for (int i = 0; i < TEXTURE_MAX; i++)
	//{
	//	
	//	g_aTree[i].load = TRUE;

	//	g_aTree[i].pos = XMFLOAT3(-50.0f + i * 30.0f, 7.0f, 20.0f);
	//	g_aTree[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	g_aTree[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);

	//	// モデルのディフューズを保存しておく。色変え対応の為。
	//	GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

	//	XMFLOAT3 pos = g_Enemy[i].pos;
	//	pos.y -= (ENEMY_OFFSET_Y - 0.1f);
	//	g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

	//	g_Enemy[i].time = 0.0f;			// 線形補間用のタイマーをクリア
	//	g_Enemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
	//	g_Enemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

	//	g_Enemy[i].use = TRUE;			// TRUE:生きてる

	//}

	//g_TexNo = 0;

	// 木ワークの初期化
	for (int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		LoadModel(MODEL_TREE, &g_aTree[nCntTree].model);
		g_aTree[nCntTree].load = TRUE;
		g_aTree[nCntTree].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_aTree[nCntTree].scl = XMFLOAT3(0.3f, 0.3f, 0.3f); // Sabit ölçek
		g_aTree[nCntTree].use = FALSE;
		GetModelDiffuse(&g_aTree[nCntTree].model, &g_aTree[nCntTree].diffuse[0]);
	}



	// 木の設定
	// === AĞAÇ YERLEŞTİRME AYARLARI ===
	const float worldSize = 1000.0f;          // Dünya sınırı: -500 ile +500
	const int maxTrees = 500;                // Yerleştirilecek ağaç sayısı
	const float minDistance = 100.0f;         // Ağaçlar arası minimum mesafe
	const int maxAttempts = 100;             // Her ağaç için maksimum deneme

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(-worldSize, worldSize);

	int placed = 0;
	while (placed < maxTrees)
	{
		bool valid = false;
		XMFLOAT3 candidatePos;

		// Maksimum deneme sayısı kadar dene
		for (int attempt = 0; attempt < maxAttempts; attempt++)
		{
			candidatePos = XMFLOAT3(dist(gen), 0.0f, dist(gen));

			// Mevcut tüm ağaçlarla çarpışma kontrolü
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
			SetTree(candidatePos,XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)); // Varsayılan renk ile yerleştir
			placed++;
		}
		else
		{
			// Yeterli boş alan yok → döngüyü kır
			break;
		}
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	



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
		// αテストを有効に
		SetAlphaTestEnable(TRUE);
	}

	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_TREE; i++)
	{
		if(g_aTree[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// 関数使った版
			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;


			// 正方行列（直交行列）を転置行列させて逆行列を作ってる版(速い)
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


			// スケールを反映
			mtxScl = XMMatrixScaling(g_aTree[i].scl.x, g_aTree[i].scl.y, g_aTree[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_aTree[i].pos.x, g_aTree[i].pos.y, g_aTree[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			DrawModel(&g_aTree[i].model);

		}

	}

	// ライティングを有効に

	SetFogEnable(TRUE);

	SetLightEnable(TRUE);
	

	// αテストを無効に
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
			g_aTree[i].nIdxShadow = CreateShadow(pos, 1.0f, 1.0f);
			return i;
		}
	}
	return -1;
}
