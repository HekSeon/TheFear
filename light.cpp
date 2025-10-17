///////////////////////////////////
// 
// light.cpp
// 
// Author: HekSeon
///////////////////////////////////
#include "main.h"
#include "renderer.h"
#include "light.h"

///////////////////////////////////
// makro area
///////////////////////////////////




/////////////////////////////////////
// prototatip area 
/////////////////////////////////////


/////////////////////////////////////
// global area 
/////////////////////////////////////
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

static BOOL		g_FogEnable = FALSE;


/////////////////////////////////////
// initlight area 
/////////////////////////////////////
void InitLight(void)
{

	
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Light[i].Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
		g_Light[i].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_Light[i].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		g_Light[i].Attenuation = 100.0f;	
		g_Light[i].Type = LIGHT_TYPE_NONE;	
		g_Light[i].Enable = TRUE;			
		SetLight(i, &g_Light[i]);
	}

	// 並行光源の設定（世界を照らす光）
	g_Light[0].Direction = XMFLOAT3(0.0f, -0.8f, -0.5f);		
	g_Light[0].Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Light[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);	
	g_Light[0].Attenuation = 1000.0f;
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					
	g_Light[0].Enable = TRUE;									
	SetLight(0, &g_Light[0]);								



	g_Fog.FogStart = 20.0f;									
	g_Fog.FogEnd = 250.0f;									
	g_Fog.FogColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); // Skydome rengi gibi
	SetFog(&g_Fog);
	SetFogEnable(TRUE);				

}


/////////////////////////////////////
// update light area 
/////////////////////////////////////
void UpdateLight(void)
{



}


/////////////////////////////////////
// light type area area 
/////////////////////////////////////
void SetLightData(int index, LIGHT* light)
{
	SetLight(index, light);
}


LIGHT* GetLightData(int index)
{
	return(&g_Light[index]);
}

/////////////////////////////////////
// fog area 
/////////////////////////////////////
void SetFogData(FOG* fog)
{
	SetFog(fog);
}


BOOL	GetFogEnable(void)
{
	return(g_FogEnable);
}


