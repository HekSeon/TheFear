#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH         (SCREEN_WIDTH)    // 背景サイズ：幅
#define TEXTURE_HEIGHT        (SCREEN_HEIGHT)   // 背景サイズ：高さ
#define TEXTURE_MAX           (6)               // テクスチャの最大数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;                // 頂点バッファ
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };    // テクスチャリソース

// テクスチャファイルパス一覧
static char* g_TexturName[TEXTURE_MAX] = {
    "data/TEXTURE/bg1.jpg",        // 0: 背景1 (通常)
    "data/TEXTURE/bg.jpg",        // 1: 背景2 (雷エフェクト等)
    "data/TEXTURE/titlelogo.png",  // 2: タイトルロゴ
    "data/TEXTURE/start.png",      // 3: スタートボタン
    "data/TEXTURE/quit.png",       // 4: 終了ボタン
	"data/TEXTURE/pointer.png",       // 5: pointer
};

static int   g_CurrentBG = 0;       // 現在表示中の背景番号
static float g_ElapsedTime = 0.0f;  // 経過時間カウント
static int   g_MenuSelect = 0;      // 現在のメニュー選択インデックス (0:Start, 1:Help, 2:Quit)
static float g_Alpha = 1.0f;        // 点滅用アルファ値
static BOOL  g_AlphaFlag = TRUE;    // アルファ値増減フラグ
static BOOL  g_Load = FALSE;        // ロード済みフラグ

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
    ID3D11Device* pDevice = GetDevice();

    // テクスチャの読み込み
    for (int i = 0; i < TEXTURE_MAX; i++)
    {
        D3DX11CreateShaderResourceViewFromFile(GetDevice(),
            g_TexturName[i], NULL, NULL, &g_Texture[i], NULL);
    }

    // 頂点バッファの作成
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

    // 変数の初期化
    g_ElapsedTime = 0.0f;
    g_MenuSelect = 0;
    g_Alpha = 1.0f;
    g_Load = TRUE;

    return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
    if (g_Load == FALSE) return;

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

    g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
    // --- 背景切り替えアニメーション ---
    g_ElapsedTime += 1.0f;
    if (g_CurrentBG == 0 && g_ElapsedTime >= 300.0f)
    {
        g_CurrentBG = 1; // 雷背景へ
        g_ElapsedTime = 0.0f;
    }
    else if (g_CurrentBG == 1 && g_ElapsedTime >= 50.0f)
    {
        g_CurrentBG = 0; // 通常背景へ戻す
        g_ElapsedTime = 0.0f;
    }

    // --- メニュー選択の入力制御 ---
    if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S))
    {
        g_MenuSelect = (g_MenuSelect + 1) % 2; // 下へ移動
    }
    else if (GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W))
    {
        g_MenuSelect = (g_MenuSelect + 2) % 2; // 上へ移動
    }

    // --- 決定操作 ---
    if (GetKeyboardTrigger(DIK_RETURN))
    {
        switch (g_MenuSelect)
        {
        case 0: // スタート
            SetFade(FADE_OUT, MODE_GAME);
            break;
        case 1: // 終了
            exit(0);
            break;
        }
    }

    // --- ロゴの点滅（アルファアニメーション） ---
    if (g_AlphaFlag)
    {
        g_Alpha -= 0.02f;
        if (g_Alpha <= 0.5f) g_AlphaFlag = FALSE;
    }
    else
    {
        g_Alpha += 0.02f;
        if (g_Alpha >= 1.0f) g_AlphaFlag = TRUE;
    }
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
    // 頂点バッファの設定
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

    // 2D描画用マトリクス設定
    SetWorldViewProjection2D();
    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // --- 1. 背景の描画 ---
    GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_CurrentBG]);
    SetSprite(g_VertexBuffer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 1);
    GetDeviceContext()->Draw(4, 0);

    // --- 2. タイトルロゴの描画 (点滅効果付き) ---
    GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
    SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, 200, 480, 80, 0, 0, 1, 1, XMFLOAT4(1, 1, 1, g_Alpha));
    GetDeviceContext()->Draw(4, 0);

    // --- 3. メニューボタンの描画 (選択中のみ強調表示) ---
    float menuX = SCREEN_WIDTH / 2;

    // ゲーム開始ボタン
    GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
    SetSpriteColor(g_VertexBuffer, menuX, 500, 240, 60, 0, 0, 1, 1,
        XMFLOAT4(1, 1, 1, (g_MenuSelect == 0 ? 1.0f : 0.4f)));
    GetDeviceContext()->Draw(4, 0);

    // ヘルプボタン
    GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
    SetSpriteColor(g_VertexBuffer, menuX, 600, 240, 60, 0, 0, 1, 1,
        XMFLOAT4(1, 1, 1, (g_MenuSelect == 1 ? 1.0f : 0.4f)));
    GetDeviceContext()->Draw(4, 0);


    // --- 4. セレクター（カーソル）の描画 ---
    // 選択されている項目の横に目印を表示
    float cursorY = 500.0f + (g_MenuSelect * 100.0f);
    GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]); // ロゴテクスチャを流用（必要に応じて変更）
    SetSpriteColor(g_VertexBuffer, menuX - 200, cursorY, 40, 40, 0, 0, 1, 1, XMFLOAT4(1, 0.5f, 1, 1));
    GetDeviceContext()->Draw(4, 0);
}