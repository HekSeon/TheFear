//=============================================================================
//
// 木処理 [tree.cpp]
// Author : BAYAR "HekSeon" SEMIH
//
//=============================================================================
#include <DirectXCollision.h>
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
	BOOL		use;			// 使用しているかどうか

} TREE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
#define MODEL_TREE "data/MODEL/tree.obj"
static ID3D11Buffer* g_TreeLodBuffer = nullptr;

static DX11_MODEL g_TreeModel{};
static TREE					g_aTree[MAX_TREE];	// 木ワーク
static BOOL					g_bAlpaTest;		// アルファテストON/OFF
//static int					g_nAlpha;			// アルファテストの閾値

namespace
{
constexpr float DetailDistance = 750.0f;
constexpr float CullDistance = 2600.0f;
constexpr XMFLOAT4 Bark = {0.21f, 0.14f, 0.06f, 1.0f};
constexpr XMFLOAT4 Leaves = {0.26f, 0.44f, 0.11f, 1.0f};

struct TreeLodVertex
{
    XMFLOAT2 position;
    XMFLOAT4 color;
};

constexpr TreeLodVertex TreeSilhouette[] = {
    {{-5, 0}, Bark}, {{5, 0}, Bark}, {{4, 53}, Bark},
    {{-5, 0}, Bark}, {{4, 53}, Bark}, {{-3, 53}, Bark},
    {{-5, 26}, Bark}, {{-29, 52}, Bark}, {{-21, 52}, Bark},
    {{3, 32}, Bark}, {{28, 58}, Bark}, {{22, 61}, Bark},
    {{-42, 46}, Leaves}, {{-31, 68}, Leaves}, {{-5, 67}, Leaves},
    {{-42, 46}, Leaves}, {{-5, 67}, Leaves}, {{-4, 48}, Leaves},
    {{-21, 56}, Leaves}, {{-12, 76}, Leaves}, {{17, 77}, Leaves},
    {{-21, 56}, Leaves}, {{17, 77}, Leaves}, {{20, 54}, Leaves},
    {{6, 48}, Leaves}, {{18, 67}, Leaves}, {{42, 57}, Leaves},
    {{6, 48}, Leaves}, {{42, 57}, Leaves}, {{34, 45}, Leaves},
};
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTree(void)
{
	LoadModel(MODEL_TREE, &g_TreeModel);

	D3D11_BUFFER_DESC lodDesc{};
	lodDesc.ByteWidth = sizeof(VERTEX_3D) * _countof(TreeSilhouette) * MAX_TREE;
	lodDesc.Usage = D3D11_USAGE_DYNAMIC;
	lodDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	lodDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&lodDesc, nullptr, &g_TreeLodBuffer);
	for (int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		g_aTree[nCntTree].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_aTree[nCntTree].scl = XMFLOAT3(0.3f, 0.3f, 0.3f); // Ağaç ölçeği
		g_aTree[nCntTree].use = FALSE; // Başlangıçta pasif
	}

	// 木の設定
	const float worldSize = 4000.0f;          // Dünya sınırı (-500 ile +500)
	const int maxTrees = 1000;                // Yerleştirilecek ağaç sayısı
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
	if (g_TreeModel.SubsetArray)
	{
		UnloadModel(&g_TreeModel);
		g_TreeModel = {};
	}

	if (g_TreeLodBuffer)
	{
		g_TreeLodBuffer->Release();
		g_TreeLodBuffer = nullptr;
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
    if (g_bAlpaTest)
        SetAlphaTestEnable(TRUE);
    SetLightEnable(FALSE);

    const CAMERA& camera = *GetCamera();
    const XMMATRIX view = XMLoadFloat4x4(&camera.mtxView);
    XMMATRIX billboard = XMLoadFloat4x4(&camera.mtxInvView);
    billboard.r[3] = XMVectorSet(0, 0, 0, 1);
    const XMVECTOR eye = XMLoadFloat3(&camera.pos);
    BoundingFrustum frustum;
    BoundingFrustum::CreateFromMatrix(frustum, XMLoadFloat4x4(&camera.mtxProjection));
    const TREE* distant[MAX_TREE];
    UINT distantCount = 0;

    for (const TREE& tree : g_aTree)
    {
        if (!tree.use) continue;
        const XMVECTOR origin = XMLoadFloat3(&tree.pos);
        const float distanceSq = XMVectorGetX(XMVector3LengthSq(origin - eye));
        if (distanceSq > CullDistance * CullDistance) continue;

        BoundingSphere bounds;
        XMStoreFloat3(&bounds.Center, XMVector3TransformCoord(origin, view) + XMVectorSet(0, 38, 0, 0));
        bounds.Radius = 72.0f;
        if (frustum.Contains(bounds) == DISJOINT) continue;

        if (g_TreeLodBuffer && distanceSq > DetailDistance * DetailDistance)
        {
            distant[distantCount++] = &tree;
            continue;
        }

        XMMATRIX world = billboard * XMMatrixScaling(tree.scl.x, tree.scl.y, tree.scl.z) *
            XMMatrixTranslationFromVector(origin);
        SetWorldMatrix(&world);
        DrawModel(&g_TreeModel);
    }

    if (distantCount > 0)
    {
        auto context = GetDeviceContext();
        D3D11_MAPPED_SUBRESOURCE mapped;
        if (SUCCEEDED(context->Map(g_TreeLodBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        {
            auto vertices = static_cast<VERTEX_3D*>(mapped.pData);
            for (UINT i = 0; i < distantCount; ++i)
            {
                const XMVECTOR origin = XMLoadFloat3(&distant[i]->pos);
                for (const TreeLodVertex& vertex : TreeSilhouette)
                {
                    const XMVECTOR local = XMVectorSet(vertex.position.x, vertex.position.y, 0, 0);
                    XMStoreFloat3(&vertices->Position, origin + XMVector3TransformNormal(local, billboard));
                    vertices->Normal = {0, 0, -1};
                    vertices->Diffuse = vertex.color;
                    vertices->TexCoord = {};
                    ++vertices;
                }
            }
            context->Unmap(g_TreeLodBuffer, 0);

            XMMATRIX world = XMMatrixIdentity();
            SetWorldMatrix(&world);
            MATERIAL material{};
            material.Diffuse = {1, 1, 1, 1};
            material.noTexSampling = 1;
            SetMaterial(material);
            SetCullingMode(CULL_MODE_NONE);
            const UINT stride = sizeof(VERTEX_3D), offset = 0;
            context->IASetVertexBuffers(0, 1, &g_TreeLodBuffer, &stride, &offset);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context->Draw(distantCount * _countof(TreeSilhouette), 0);
            SetCullingMode(CULL_MODE_BACK);
        }
    }

    SetLightEnable(TRUE);
    SetBlendState(BLEND_MODE_ALPHABLEND);
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