///////////////////////////////////
// 
// main.cpp
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "meshwall.h"
#include "collision.h"
#include "sound.h"
#include "game.h"
#include "fade.h"
#include "title.h"
#include "result.h"

// ImGui kullanılıyorsa ileride aktif edilebilir
//#include "imgui.h"
//#include "imgui_impl_win32.h"
//#include "imgui_impl_dx11.h"

//*****************************************************************************
// マクロ定義 (Macros)
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウィンドウのクラス名
#define WINDOW_NAME		"メッシュ表示"		// ウィンドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
void CheckHit(void);

//*****************************************************************************
// グローバル変数 (Global Variables)
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;

#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンター
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用
#endif

int g_Mode = MODE_TITLE;

//=============================================================================
// メイン関数 (Main Function)
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 未使用宣言による警告防止
	UNREFERENCED_PARAMETER(lpCmdLine);		// 未使用宣言による警告防止

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// 初期化処理 (ウィンドウを作成してから行う)
	if (FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウィンドウの表示 (初期化処理の後に行う)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{ // PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存
#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ (Window Procedure)
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitLight();

	InitSound(hWnd);

	InitCamera();

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	SetLightEnable(TRUE);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);

	InitFade();

	SetMode(g_Mode);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	SetMode(MODE_MAX);

	UninitSound();

	// カメラの終了処理
	UninitCamera();

	// 入力の終了処理
	UninitInput();

	// レンダラーの終了処理
	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	UpdateInput();

	UpdateLight();

	// カメラ更新
	UpdateCamera();

	switch (g_Mode)
	{
	case MODE_TITLE:
		UpdateTitle();
		break;
	case MODE_GAME:
		UpdateGame();
		break;
	case MODE_RESULT:
		UpdateResult();
		break;
	}

	UpdateFade();
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	Clear();

	SetCamera();

	switch (g_Mode)
	{
	case MODE_TITLE:
		SetViewPort(TYPE_FULL_SCREEN);
		SetDepthEnable(FALSE);
		SetLightEnable(FALSE);

		DrawTitle();

		SetLightEnable(TRUE);
		SetDepthEnable(TRUE);
		break;

	case MODE_GAME:
		DrawGame();
#ifdef _DEBUG
		PrintDebugProc("FPS:%d\n", g_CountFPS);
#endif
		break;

	case MODE_RESULT:
		SetViewPort(TYPE_FULL_SCREEN);
		SetDepthEnable(FALSE);
		SetLightEnable(FALSE);

		DrawResult();

		SetLightEnable(TRUE);
		SetDepthEnable(TRUE);
		break;
	}

	// フェード処理の描画
	{
		SetViewPort(TYPE_FULL_SCREEN);
		SetDepthEnable(FALSE);
		SetLightEnable(FALSE);

		DrawFade();

		SetDepthEnable(TRUE);
		SetLightEnable(TRUE);
	}

#ifdef _DEBUG
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
	Present();
}

long GetMousePosX(void)
{
	return g_MouseX;
}

long GetMousePosY(void)
{
	return g_MouseY;
}

#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif

//=============================================================================
// ゲームのモードを変える
//=============================================================================
void SetMode(int mode)
{
	UninitGame();

	g_Mode = mode;

	switch (g_Mode)
	{
	case MODE_TITLE:
		UninitTitle();
		InitTitle();
		break;

	case MODE_GAME:
		UninitCamera();
		InitCamera();
		InitGame();
		break;

	case MODE_RESULT:
		UninitResult();
		InitResult();
		break;

	case MODE_MAX:
		UninitPlayer();
		break;
	}
}

int GetMode(void)
{
	return g_Mode;
}