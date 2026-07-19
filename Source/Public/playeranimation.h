///////////////////////////////////
// 
// playeranimation.h
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#pragma once

#include <DirectXMath.h>

// Yapısal döngüyü engellemek için ön bildirim[cite: 7, 8]
struct PLAYER;

using namespace DirectX;

//============================================================================

enum PLAYER_ANIM
{
    ANIM_IDLE = 0,
    ANIM_WALK,
    ANIM_ATTACK
};

//============================================================================

struct INTERPOLATION_DATA
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 rot;
    DirectX::XMFLOAT3 scl;
    int frame;
};

//============================================================================
void InitPlayerAnimation(PLAYER* player);
void UpdatePlayerAnimation(PLAYER* player);

void SetPlayerAnimation(PLAYER* player, PLAYER_ANIM anim);