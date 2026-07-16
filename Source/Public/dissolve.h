///////////////////////////////////
// 
// light.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////

#pragma once

#include "main.h"

#define _DISSOLVE_H_

enum DISSOLVE_MODE
{
	DISSOLVE_NONE,
	DISSOLVE_OUT,
	DISSOLVE_IN
};

struct DISSOLVE_STATE
{
	float threshold;
	float minY;
	float maxY;
	DISSOLVE_MODE mode;
	float speed;
};

//============================

void StartDissolveOut(DISSOLVE_STATE* state, float speed, float minY, float maxY);
void StartDissolveIn(DISSOLVE_STATE* state, float speed, float minY, float maxY);
bool bUpdateDdissolve(DISSOLVE_STATE* state, float deltatime);



