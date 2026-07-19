///////////////////////////////////
// 
// renderer.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "main.h" // XMFLOAT tipleri, XMMATRIX ve Windows handle'ları için şart[cite: 2, 9]

//*********************************************************
// マクロ定義
//*********************************************************
#define LIGHT_MAX		(5)

enum LIGHT_TYPE
{
	LIGHT_TYPE_NONE,		//ライト無し[cite: 9]
	LIGHT_TYPE_DIRECTIONAL,	//ディレクショナルライト[cite: 9]
	LIGHT_TYPE_POINT,		//ポイントライト[cite: 9]

	LIGHT_TYPE_NUM
};

enum BLEND_MODE
{
	BLEND_MODE_NONE,		//ブレンド無し[cite: 9]
	BLEND_MODE_ALPHABLEND,	//αブレンド[cite: 9]
	BLEND_MODE_ADD,			//加算ブレンド[cite: 9]
	BLEND_MODE_SUBTRACT,	//減算ブレンド[cite: 9]

	BLEDD_MODE_NUM
};

enum CULL_MODE
{
	CULL_MODE_NONE,			//カリング無し[cite: 9]
	CULL_MODE_FRONT,		//表のポリゴンを描画しない(CW)[cite: 9]
	CULL_MODE_BACK,			//裏のポリゴンを描画しない(CCW)[cite: 9]

	CULL_MODE_NUM
};

//*********************************************************
// 構造体
//*********************************************************

// 頂点構造体[cite: 9]
struct VERTEX_3D
{
	XMFLOAT3	Position;
	XMFLOAT3	Normal;
	XMFLOAT4	Diffuse;
	XMFLOAT2	TexCoord;
};

// マテリアル構造体[cite: 9]
struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
};

// ライト構造体[cite: 9]
struct LIGHT {
	XMFLOAT3	Direction;	// ライトの方向[cite: 9]
	XMFLOAT3	Position;	// ライトの位置[cite: 9]
	XMFLOAT4	Diffuse;	// 拡散光の色[cite: 9]
	XMFLOAT4	Ambient;	// 環境光の色[cite: 9]
	float		Attenuation;// 減衰率[cite: 9]
	int			Type;		// ライト種別・有効フラグ[cite: 9]
	int			Enable;		// ライト種別・有効フラグ[cite: 9]
};

// フォグ構造体[cite: 9]
struct FOG {
	float		FogStart;	// フォグの開始距離[cite: 9]
	float		FogEnd;		// フォグの最大距離[cite: 9]
	XMFLOAT4	FogColor;	// フォグの色[cite: 9]
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void UninitRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device* GetDevice(void);
ID3D11DeviceContext* GetDeviceContext(void);

void SetDepthEnable(BOOL Enable);
void SetBlendState(BLEND_MODE bm);
void SetCullingMode(CULL_MODE cm);
void SetAlphaTestEnable(BOOL flag);

void SetWorldViewProjection2D(void);
void SetWorldMatrix(XMMATRIX* WorldMatrix);
void SetViewMatrix(XMMATRIX* ViewMatrix);
void SetProjectionMatrix(XMMATRIX* ProjectionMatrix);

void SetMaterial(MATERIAL material);

void SetLightEnable(BOOL flag);
void SetLight(int index, LIGHT* light);

void SetFogEnable(BOOL flag);
void SetFog(FOG* fog);

void DebugTextOut(char* text, int x, int y);

void SetFuchi(int flag);
void SetShaderCamera(XMFLOAT3 pos);

void SetClearColor(float* color4);

void SetDissolve(float threshold, float minY, float maxY, bool dissolveEnable);