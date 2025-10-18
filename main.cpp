//=============================================================================
//
// ���C������ [main.cpp]
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
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"���b�V���\��"		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int g_Mode = MODE_TITLE;



//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
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
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// �E�B���h�E�̍����W
		CW_USEDEFAULT,																		// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
	
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��



				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
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
// ����������
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
//	io.Fonts->AddFontFromFileTTF(u8"c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());    // ���{��t�H���g�̎w��
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

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	SetLightEnable(TRUE);

	// �w�ʃ|���S�����J�����O
	SetCullingMode(CULL_MODE_BACK);

	InitFade();

	SetMode(g_Mode);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// �I���̃��[�h���Z�b�g

//#ifdef _DEBUG
//	ImGui_ImplDX11_Shutdown();
//	ImGui_ImplWin32_Shutdown();
//	ImGui::DestroyContext();
//#endif


	SetMode(MODE_MAX);

	UninitSound()
		;
	// �J�����̏I������
	UninitCamera();

	//���͂̏I������
	UninitInput();

	// �����_���[�̏I������
	UninitRenderer();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	/*ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();*/
	// ���͂̍X�V����
	UpdateInput();

	UpdateLight();

	// �J�����X�V
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
// �`�揈��
//=============================================================================

// 3D�`�敨�̊�{�Z�b�g
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

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
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
// �Q�[���̃��[�h��ς��
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


