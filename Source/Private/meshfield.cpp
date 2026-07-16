//=============================================================================
//
// メッシュ地面の処理 [meshfield.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "meshfield.h"
#include "renderer.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX         (1)                // テクスチャの数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;    // 頂点バッファ
static ID3D11Buffer* g_IndexBuffer = NULL;     // インデックスバッファ

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };    // テクスチャ情報
static int                             g_TexNo;                   // テクスチャ番号

static XMFLOAT3        g_posField;                                // ポリゴン表示位置の中心座標
static XMFLOAT3        g_rotField;                                // ポリゴンの回転角

static int            g_nNumBlockXField, g_nNumBlockZField;      // ブロック数
static int            g_nNumVertexField;                         // 総頂点数    
static int            g_nNumVertexIndexField;                    // 総インデックス数
static int            g_nNumPolygonField;                        // 総ポリゴン数
static float          g_fBlockSizeXField, g_fBlockSizeZField;    // ブロックサイズ

static char* g_TextureName[] = {
    "data/TEXTURE/field004.jpg",
    "data/TEXTURE/field002.jpg",
};

static VERTEX_3D* g_Vertex = NULL;
static BOOL         g_Load = FALSE;                              // 多重解放・初期化防止フラグ

static float GenerateMountainHeight(float x, float z)
{
    float height = 0.0f;
    height += sinf(x * 0.008f) * cosf(z * 0.008f) * 90.0f;
    height += sinf(x * 0.025f + z * 0.03f) * 30.0f;
    height += sinf(x * 0.07f) * cosf(z * 0.065f) * 8.0f;
    return height;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
    int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
    if (g_Load) return S_OK; // すでに初期化されている場合はスキップ

    // ポリゴン表示位置の中心座標を設定
    g_posField = pos;
    g_rotField = rot;

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

    g_TexNo = 0;

    // ブロック数の設定
    g_nNumBlockXField = nNumBlockX;
    g_nNumBlockZField = nNumBlockZ;

    // 頂点数の設定
    g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

    // インデックス数の設定
    g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

    // ポリゴン数の設定
    g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

    // ブロックサイズの設定
    g_fBlockSizeXField = fBlockSizeX;
    g_fBlockSizeZField = fBlockSizeZ;

    // 頂点情報メモリの確保
    g_Vertex = new VERTEX_3D[g_nNumVertexField];

    // 1. 頂点の位置座標と初期化設定（X, Z軸の決定後に高さを安全に計算）
    for (int z = 0; z < (g_nNumBlockZField + 1); z++)
    {
        for (int x = 0; x < (g_nNumBlockXField + 1); x++)
        {
            int idx = z * (g_nNumBlockXField + 1) + x;

            // X, Z位置の決定
            g_Vertex[idx].Position.x = -(g_nNumBlockXField / 2.0f) * g_fBlockSizeXField + x * g_fBlockSizeXField;
            g_Vertex[idx].Position.z = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField - z * g_fBlockSizeZField;

            // 静的な山の高さを計算
            g_Vertex[idx].Position.y = GenerateMountainHeight(g_Vertex[idx].Position.x, g_Vertex[idx].Position.z);

            // 一時的なデフォルト法線 (下段の法線計算ループで上書きされます)
            g_Vertex[idx].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

            // 反射光（カラー）
            g_Vertex[idx].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

            // テクスチャ座標
            float texSizeX = 1.0f;
            float texSizeZ = 1.0f;
            g_Vertex[idx].TexCoord.x = texSizeX * x;
            g_Vertex[idx].TexCoord.y = texSizeZ * z;
        }
    }

    float* tempHeights = new float[g_nNumVertexField];
    for (int z = 0; z <= g_nNumBlockZField; z++)
    {
        for (int x = 0; x <= g_nNumBlockXField; x++)
        {
            float sum = 0.0f;
            int count = 0;

            for (int dz = -1; dz <= 1; dz++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    int nz = z + dz;
                    int nx = x + dx;

                    if (nx >= 0 && nx <= g_nNumBlockXField && nz >= 0 && nz <= g_nNumBlockZField)
                    {
                        int neighborIdx = nz * (g_nNumBlockXField + 1) + nx;
                        sum += g_Vertex[neighborIdx].Position.y;
                        count++;
                    }
                }
            }
            int idx = z * (g_nNumBlockXField + 1) + x;
            tempHeights[idx] = sum / (float)count;
        }
    }

    float smoothFactor = 0.8f;
    for (int i = 0; i < g_nNumVertexField; i++)
    {
        g_Vertex[i].Position.y = g_Vertex[i].Position.y * (1.0f - smoothFactor) + tempHeights[i] * smoothFactor;
    }
    delete[] tempHeights;


    // 3. 静的な法線の計算 (Yumuşatılmış yüksekliğe göre normal vektörler hesaplanır)
    for (int z = 0; z <= g_nNumBlockZField; z++)
    {
        for (int x = 0; x <= g_nNumBlockXField; x++)
        {
            int idx = z * (g_nNumBlockXField + 1) + x;
            int idxL = z * (g_nNumBlockXField + 1) + max(x - 1, 0);
            int idxR = z * (g_nNumBlockXField + 1) + min(x + 1, g_nNumBlockXField);
            int idxU = max(z - 1, 0) * (g_nNumBlockXField + 1) + x;
            int idxD = min(z + 1, g_nNumBlockZField) * (g_nNumBlockXField + 1) + x;

            XMFLOAT3 normal;
            normal.x = g_Vertex[idxL].Position.y - g_Vertex[idxR].Position.y;
            normal.y = 2.0f * g_fBlockSizeXField;
            normal.z = g_Vertex[idxU].Position.y - g_Vertex[idxD].Position.y;

            XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&normal));
            XMStoreFloat3(&g_Vertex[idx].Normal, n);
        }
    }

    // 頂点バッファ生成
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexField;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

    // インデックスバッファ生成
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexField;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    GetDevice()->CreateBuffer(&bd, NULL, &g_IndexBuffer);

    // 頂点バッファの中身を埋める
    {
        D3D11_MAPPED_SUBRESOURCE msr;
        GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
        VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;
        memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * g_nNumVertexField);
        GetDeviceContext()->Unmap(g_VertexBuffer, 0);
    }

    // インデックスバッファの中身を埋める
    {
        D3D11_MAPPED_SUBRESOURCE msr;
        GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

        unsigned short* pIdx = (unsigned short*)msr.pData;
        int nCntIdx = 0;

        for (int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZField; nCntVtxZ++)
        {
            if (nCntVtxZ > 0)
            {
                pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1);
                nCntIdx++;
            }

            for (int nCntVtxX = 0; nCntVtxX < (g_nNumBlockXField + 1); nCntVtxX++)
            {
                pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1) + nCntVtxX;
                nCntIdx++;
                pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX;
                nCntIdx++;
            }

            if (nCntVtxZ < (g_nNumBlockZField - 1))
            {
                pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + g_nNumBlockXField;
                nCntIdx++;
            }
        }

        GetDeviceContext()->Unmap(g_IndexBuffer, 0);
    }

    g_Load = TRUE; // 初期化完了
    return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
    if (g_Load == FALSE) return; // 多重解放を防止 (g_Loadチェック)

    // インデックスバッファの解放
    if (g_IndexBuffer) {
        g_IndexBuffer->Release();
        g_IndexBuffer = NULL;
    }

    // 頂点バッファの解放
    if (g_VertexBuffer) {
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

    if (g_Vertex)
    {
        delete[] g_Vertex;
        g_Vertex = NULL;
    }

    g_Load = FALSE; // フラグをリセット
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{
    if (g_Load == FALSE) return;

    // 地面はスタティックな山岳地形になったため、毎フレームCPU上で頂点を再計算して
    // GPUへ再送受信（Map/Unmap）する必要はありません。
    // これにより無用なCPU-GPU間のボトルネックを完全に回避しています。
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField(void)
{
    if (g_Load == FALSE) return;

    // 頂点バッファ設定
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

    // インデックスバッファ設定
    GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // プリミティブトポロジ設定
    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // マテリアル設定
    MATERIAL material;
    ZeroMemory(&material, sizeof(material));
    material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
    SetMaterial(material);

    // テクスチャ設定
    GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

    XMMATRIX mtxRot, mtxTranslate, mtxWorld;

    // ワールドマトリックスの初期化
    mtxWorld = XMMatrixIdentity();

    // 回転を反映
    mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
    mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

    // 移動を反映
    mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
    mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

    // ワールドマトリックスの設定
    SetWorldMatrix(&mtxWorld);

    // ポリゴンの描画
    GetDeviceContext()->DrawIndexed(g_nNumVertexIndexField, 0, 0);
}

//=============================================================================
// レイキャスト処理（地形との当たり判定）
//=============================================================================
BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3* HitPosition, XMFLOAT3* Normal)
{
    if (g_Load == FALSE) return FALSE;

    XMFLOAT3 start = pos;
    XMFLOAT3 end = pos;

    // 原点だった場合のテスト
    if (start.x == 0.0f)
    {
        start.x += 1.0f;
        end.x += 1.0f;
    }
    if (start.z == 0.0f)
    {
        start.z -= 1.0f;
        end.z -= 1.0f;
    }

    // 少し上から、下方向へレイを飛ばす
    start.y = 1000.0f;
    end.y = -1000.0f;

    // 座標から現在いるブロックを割り出し、対象のポリゴンに絞って判定（高速化）
    float fx = (g_nNumBlockXField / 2.0f) * g_fBlockSizeXField;
    float fz = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField;
    int sz = (int)((-start.z + fz) / g_fBlockSizeZField);
    int sx = (int)((start.x + fx) / g_fBlockSizeXField);
    int ez = sz + 1;
    int ex = sx + 1;

    if ((sz < 0) || (sz > g_nNumBlockZField - 1) ||
        (sx < 0) || (sx > g_nNumBlockXField - 1))
    {
        *Normal = { 0.0f, 1.0f, 0.0f };
        return FALSE;
    }

    // 周辺ブロックのみ判定
    for (int z = sz; z < ez; z++)
    {
        for (int x = sx; x < ex; x++)
        {
            XMFLOAT3 p0 = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position;
            XMFLOAT3 p1 = g_Vertex[z * (g_nNumBlockXField + 1) + (x + 1)].Position;
            XMFLOAT3 p2 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + x].Position;
            XMFLOAT3 p3 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + (x + 1)].Position;

            // 三角形ポリゴン2枚分で当たり判定
            BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, Normal);
            if (ans)
            {
                return TRUE;
            }

            ans = RayCast(p1, p2, p3, start, end, HitPosition, Normal);
            if (ans)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}
