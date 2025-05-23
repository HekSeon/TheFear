/////////////////////////////////////
// 
// camera.h
// 
// Author : HekSeon
// 
///////////////////////////////////
#pragma once


/////////////////////////////////////
// include area 
///////////////////////////////////
#include "renderer.h"

/////////////////////////////////////
// makro area 
///////////////////////////////////
struct CAMERA
{
	XMFLOAT4X4			mtxView;		
	XMFLOAT4X4			mtxInvView;		
	XMFLOAT4X4			mtxProjection;	

	XMFLOAT3			pos;			
	XMFLOAT3			at;				
	XMFLOAT3			up;				
	XMFLOAT3			rot;
	XMFLOAT3			Eye;
	XMFLOAT3			Target;

	float				len;			
	XMFLOAT3			dir;

};


enum {
	TYPE_FULL_SCREEN,
	TYPE_LEFT_HALF_SCREEN,
	TYPE_RIGHT_HALF_SCREEN,
	TYPE_UP_HALF_SCREEN,
	TYPE_DOWN_HALF_SCREEN,
	TYPE_NONE,

};

enum CameraMode {
    FIRST_PERSON,
    THIRD_PERSON
};

static CameraMode g_CameraMode = THIRD_PERSON;


/////////////////////////////////////
// protocol area 
///////////////////////////////////
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);

CAMERA* GetCamera(void);

void SetViewPort(int type);
int GetViewPortType(void);

void SetCameraAT(XMFLOAT3 pos, int x, int y, int z);


void ProcessMouseMovement();
void UpdateCameraWithMouse();
void ChangeCameraMode(int mode);

void DrawTPSPlayer(void);
