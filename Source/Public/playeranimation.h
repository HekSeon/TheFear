#pragma once

#include <DirectXMath.h>
using namespace DirectX;


enum PLAYER_ANIM
{
    ANIM_IDLE = 0,
    ANIM_WALK,
    ANIM_ATTACK
};


struct INTERPOLATION_DATA
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 rot;
    DirectX::XMFLOAT3 scl;
    int frame;
};


void InitPlayerAnimation(PLAYER* player);
void UpdatePlayerAnimation(PLAYER* player);

void SetPlayerAnimation(PLAYER* player, PLAYER_ANIM anim);

