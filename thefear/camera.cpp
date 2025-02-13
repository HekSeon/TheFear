/////////////////////////////////////
// 
// camera.cpp
// 
// Author : HekSeon
// 
///////////////////////////////////
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"

/////////////////////////////////////
// makro area 
///////////////////////////////////
#define	POS_X_CAM			(0.0f)		
#define	POS_Y_CAM			(50.0f)			
#define	POS_Z_CAM			(-140.0f)		

//#define	POS_X_CAM		(0.0f)			
//#define	POS_Y_CAM		(200.0f)		
//#define	POS_Z_CAM		(-400.0f)		


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))					
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	
#define	VIEW_NEAR_Z		(10.0f)										
#define	VIEW_FAR_Z		(10000.0f)	// ビュー平面のFarZ値



#define	VALUE_MOVE_CAMERA	(2.0f)										
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)								

/////////////////////////////////////
// global area 
///////////////////////////////////
static CAMERA			g_Camera;		

static int				g_ViewPortType = TYPE_FULL_SCREEN;

static int lastMouseX = SCREEN_WIDTH / 2;
static int lastMouseY = SCREEN_HEIGHT / 2;

static int deltaX;
static int deltaY;



static int cameraMode = THIRD_PERSON; // Varsayılan: 3rd Person

/////////////////////////////////////
// init camera area 
///////////////////////////////////
void InitCamera(void)
{
	ChangeCameraMode(cameraMode);
}


/////////////////////////////////////
// uninit area area 
///////////////////////////////////
void UninitCamera(void)
{

}


/////////////////////////////////////
// update camera area 
///////////////////////////////////
void UpdateCamera(void)
{
	if (GetKeyboardTrigger(DIK_X))
	{
		ChangeCameraMode(FIRST_PERSON);
	}

	if (GetKeyboardTrigger(DIK_C))
	{
		ChangeCameraMode(THIRD_PERSON);
	}

	PLAYER* player = GetPlayer();
	player->spd = 0.0f;

	UpdateCameraWithMouse();
	/*ShowCursor(FALSE);*/

#ifdef _DEBUG


	

	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}



#endif



#ifdef _DEBUG	
	char debugInfo[128];

	OutputDebugStringA(debugInfo);

#endif
}


/////////////////////////////////////
// set camera area 
///////////////////////////////////
void SetCamera(void)
{
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera.mtxInvView, mtxInvView);

	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera.pos);
}


/////////////////////////////////////
// get camera area 
///////////////////////////////////
CAMERA* GetCamera(void)
{
	return &g_Camera;
}

/////////////////////////////////////
// viewport area 
///////////////////////////////////
void SetViewPort(int type)
{
	ID3D11DeviceContext* g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	g_ViewPortType = type;

	switch (g_ViewPortType)
	{
	case TYPE_FULL_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_LEFT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_RIGHT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = (FLOAT)SCREEN_WIDTH / 2;
		vp.TopLeftY = 0;
		break;

	case TYPE_UP_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_DOWN_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = (FLOAT)SCREEN_HEIGHT / 2;
		break;


	}
	g_ImmediateContext->RSSetViewports(1, &vp);

}


int GetViewPortType(void)
{
	return g_ViewPortType;
}

void SetCameraAT(XMFLOAT3 pos)
{
	ProcessMouseMovement();

	g_Camera.at = pos;

	g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
	g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	//g_Camera.pos.y = g_Camera.at.y;
}



void ProcessMouseMovement() {
	static bool firstMouse = true;
	static bool isMouseLocked = false; // Fare kilitli mi kontrolü
	const float mouseSensitivity = 0.002f; // Fare hassasiyeti

	POINT mousePos;
	GetCursorPos(&mousePos);

	int centerX = SCREEN_WIDTH / 2;
	int centerY = SCREEN_HEIGHT / 2;

	// F1 tuşuna basılırsa fare serbest bırak
	if (GetKeyboardTrigger(DIK_F1)) {
		ClipCursor(NULL);
		ShowCursor(TRUE);
		isMouseLocked = false;
		return;
	}
	// F2 tuşuna basılırsa fareyi kilitle
	else if (GetKeyboardTrigger(DIK_F2)) {
		ShowCursor(FALSE);
		isMouseLocked = true;
		SetCursorPos(centerX, centerY);
	}

	// Eğer fare serbestse hareketi işlemeden çık
	if (!isMouseLocked) return;

	// İlk karede fareyi sıfırla
	if (firstMouse) {
		lastMouseX = centerX;
		lastMouseY = centerY;
		SetCursorPos(centerX, centerY);
		firstMouse = false;
		return;
	}

	// Farenin hareket farklarını hesapla
	int deltaX = mousePos.x - lastMouseX;
	int deltaY = mousePos.y - lastMouseY;

	// Hassasiyet uygula
	g_Camera.rot.y += deltaX * mouseSensitivity;
	g_Camera.rot.x -= deltaY * mouseSensitivity; // Y ekseni ters olduğu için çıkartıyoruz

	// Yukarı-aşağı sınırlandırma
	if (g_Camera.rot.x > XM_PI / 2.0f) g_Camera.rot.x = XM_PI / 2.0f - 0.01f;
	if (g_Camera.rot.x < -XM_PI / 2.0f) g_Camera.rot.x = -XM_PI / 2.0f + 0.01f;

	// Farenin merkezlenmesi (FPS kontrolü için)
	SetCursorPos(centerX, centerY);
	lastMouseX = centerX;
	lastMouseY = centerY;
}



void ChangeCameraMode(int mode)
{
	cameraMode = mode;

	if (mode == THIRD_PERSON) {
		g_Camera.pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
		g_Camera.at = { 0.0f, 0.0f, 0.0f };
		g_Camera.up = { 0.0f, 1.0f, 0.0f };
		g_Camera.rot = { 0.0f, 0.0f, 0.0f };

		float vx = g_Camera.pos.x - g_Camera.at.x;
		float vz = g_Camera.pos.z - g_Camera.at.z;
		g_Camera.len = sqrtf(vx * vx + vz * vz);
	}
	else if (mode == FIRST_PERSON) {
		g_Camera.pos = { POS_X_CAM, POS_Y_CAM + 15.0f, POS_Z_CAM };
		g_Camera.at = { POS_X_CAM + sinf(g_Camera.rot.y), POS_Y_CAM + 15.0f, POS_Z_CAM + cosf(g_Camera.rot.y) };
		g_Camera.up = { 0.0f, 1.0f, 0.0f };
		g_Camera.rot = { 0.0f, 0.0f, 0.0f };
		g_Camera.len = 0.0f;
	}

	// Update player's animation data based on camera mode
	PLAYER* player = GetPlayer();
	player->tblNo = (mode == FIRST_PERSON) ? 0 : 1; // Example: 0 for idle, 1 for move

	SetViewPort(g_ViewPortType);
}

void UpdateCameraWithMouse() {


	//// Kameranın "at" yönünü güncelle
	//g_Camera.at.x = g_Camera.pos.x + cosf(g_Camera.rot.x) * sinf(g_Camera.rot.y);
	//g_Camera.at.y = g_Camera.pos.y + sinf(g_Camera.rot.x);
	//g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.x) * cosf(g_Camera.rot.y);
}