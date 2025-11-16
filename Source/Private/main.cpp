//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 
//
//=============================================================================
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

//#include "imgui.h"
//#include "imgui_impl_win32.h"
//#include "imgui_impl_dx11.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"メッシュ表示"		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int g_Mode = MODE_TITLE;



//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

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

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
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
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// (Your code process Win32 messages)
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
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
//
//#ifdef _DEBUG
//	// Setup Dear ImGui context
//	IMGUI_CHECKVERSION();
//	ImGui::CreateContext();
//	ImGuiIO& io = ImGui::GetIO();
//	io.Fonts->AddFontFromFileTTF(u8"c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());    // 日本語フォントの指定
//	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//
//
//	// Setup Platform/Renderer backends
//	ImGui_ImplWin32_Init(hWnd);
//	ImGui_ImplDX11_Init(GetDevice(), GetDeviceContext());
//#endif

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
	// 終了のモードをセット

//#ifdef _DEBUG
//	ImGui_ImplDX11_Shutdown();
//	ImGui_ImplWin32_Shutdown();
//	ImGui::DestroyContext();
//#endif


	SetMode(MODE_MAX);

	UninitSound()
		;
	// カメラの終了処理
	UninitCamera();

	//入力の終了処理
	UninitInput();

	// レンダラーの終了処理
	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	/*ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();*/
	// 入力の更新処理
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


//#ifdef _DEBUG
//	CAMERA* g_camera = GetCamera();
//	PLAYER* g_player = GetPlayer();
//	// (Your code process and dispatch Win32 messages)
//	// Start the Dear ImGui frame
//	if (ImGui::Begin("debugger")) {
//		ImGui::Text(" FPS:%d", g_CountFPS);
//		ImGui::Text("Mouse PosX: %d Mouse PosY : %d", GetMousePosX(), GetMousePosY());
//		ImGui::Text("GAME MODE: %d", g_Mode);
//		ImGui::Text("Player Pos: X: %d Z:%d", g_player[0].pos.x, g_player[0].pos.z);
//		ImGui::Text("Camera Pos: X:%d Y:%d Z:%d", g_camera->pos.x,g_camera->pos.y,g_camera->pos.z);
//		ImGui::Text("Camera Rot: X: %d Z: %d", g_camera->rot.x, g_camera->rot.z);
//
//		if (ImGui::Button("TITLE", ImVec2(120, 40)))
//		{
//			g_Mode = MODE_TITLE;
//		}
//
//		if (ImGui::Button("GAME", ImVec2(120, 40)))
//		{
//			g_Mode = MODE_GAME;
//		}
//
//		if (ImGui::Button("RESULT", ImVec2(120, 40)))
//		{
//			g_Mode = MODE_RESULT;
//		}
//
//		if (ImGui::Button("KILL BUTON", ImVec2(120, 40)))
//		{
//			exit(0);
//		}
//		
//	}
//	ImGui::End();
//
//#endif
}

//=============================================================================
// 描画処理
//=============================================================================

// 3D描画物の基本セット
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
// ゲームのモードを変わる
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


