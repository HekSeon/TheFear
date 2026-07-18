#pragma once
#include "main.h"

HRESULT InitSea(XMFLOAT3 centerPos, int nTilesX, int nTilesZ,
    int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ);
void UninitSea(void);
void UpdateSea(void);
void DrawSea(void);