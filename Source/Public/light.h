///////////////////////////////////
// 
// light.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include "renderer.h" // LIGHT ve FOG yapılarının tanınması için gerekli[cite: 1]

///////////////////////////////////
// makro area
///////////////////////////////////

/////////////////////////////////////
// prototatip area 
/////////////////////////////////////
void InitLight(void);
void UpdateLight(void);

void SetLightData(int index, LIGHT* light);
void SetFogData(FOG* fog);
BOOL GetFogEnable(void);

LIGHT* GetLightData(int index);