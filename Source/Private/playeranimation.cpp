#include "player.h"
#include "PlayerAnimation.h"
#include <DirectXMath.h>
using namespace DirectX;

//=================================================================
// Player.cpp'teki move/idle tablolarını buraya taşıyoruz
//=================================================================
static PLAYER_ANIM currentAnim = ANIM_IDLE;

// Idle tablolar
// pos          rot         scl         frame
static INTERPOLATION_DATA idle_tbl_head[] = {
    { {0,0,0}, {0,0,0}, {1,1,1}, 30 },
    { {0,0.1f,0}, {0,0,0}, {1,1.05f,1}, 60 },
    { {0,0,0}, {0,0,0}, {1,1,1}, 90 },
};

static INTERPOLATION_DATA idle_tbl_larm[] = {
    { {0,3,0}, {0,0,0}, {1,1,1}, 15 },
    { {0,3,0}, {0,0,0}, {1,1,1}, 30 },
};

static INTERPOLATION_DATA idle_tbl_rarm[] = {
    { {0,3,0}, {0,0,0}, {1,1,1}, 15 },
    { {0,3,0}, {0,0,0}, {1,1,1}, 30 },
};

static INTERPOLATION_DATA idle_tbl_lleg[] = {
    { {0,0,0}, {0,0,0}, {1,1,1}, 15 },
    { {0,0,0}, {0,0,0}, {1,1,1}, 30 },
};

static INTERPOLATION_DATA idle_tbl_rleg[] = {
    { {0,0,0}, {0,0,0}, {1,1,1}, 15 },
    { {0,0,0}, {0,0,0}, {1,1,1}, 30 },
};

// Move tablolar
static INTERPOLATION_DATA move_tbl_head[] = {
    { {0,0,0}, {0,0,0}, {1,1,1}, 30 },
    { {0,0.1f,0}, {0,0,0}, {1,1.05f,1}, 60 },
    { {0,0,0}, {0,0,0}, {1,1,1}, 90 },
};

static INTERPOLATION_DATA move_tbl_larm[] = {
    { {0,3,0}, {XM_PI / 9,0,0}, {1,1,1}, 15 },
    { {0,3,0}, {-XM_PI / 9,0,0}, {1,1,1}, 30 },
};

static INTERPOLATION_DATA move_tbl_rarm[] = {
    { {0,3,0}, {-XM_PI / 9,0,0}, {1,1,1}, 15 },
    { {0,3,0}, {XM_PI / 9,0,0}, {1,1,1}, 30 },
};

static INTERPOLATION_DATA move_tbl_lleg[] = {
    { {0,0,0}, {-XM_PI / 9,0,0}, {1,1,1}, 15 },
    { {0,0,0}, {XM_PI / 9,0,0}, {1,1,1}, 30 },
};

static INTERPOLATION_DATA move_tbl_rleg[] = {
    { {0,0,0}, {XM_PI / 9,0,0}, {1,1,1}, 15 },
    { {0,0,0}, {-XM_PI / 9,0,0}, {1,1,1}, 30 },
};

// Tablo adresleri
static INTERPOLATION_DATA* g_IdleTblAdr[] = {
    idle_tbl_head, idle_tbl_larm, idle_tbl_rarm, idle_tbl_lleg, idle_tbl_rleg
};

static INTERPOLATION_DATA* g_MoveTblAdr[] = {
    move_tbl_head, move_tbl_larm, move_tbl_rarm, move_tbl_lleg, move_tbl_rleg
};

//=================================================================
// Animasyon yönetimi
//=================================================================
void InitPlayerAnimation(PLAYER* player)
{
    if (!player) return;

    currentAnim = ANIM_IDLE;

    for (int i = 0; i < PLAYER_PARTS_MAX; i++)
    {
        PLAYER_PART& part = player->parts[i];
        part.time = 0.0f;
        part.tblNo = i;
        part.use = TRUE;

        // tblMax başlangıç
        switch (i)
        {
        case 0: part.tblMax = sizeof(idle_tbl_head) / sizeof(INTERPOLATION_DATA); break;
        case 1: part.tblMax = sizeof(idle_tbl_larm) / sizeof(INTERPOLATION_DATA); break;
        case 2: part.tblMax = sizeof(idle_tbl_rarm) / sizeof(INTERPOLATION_DATA); break;
        case 3: part.tblMax = sizeof(idle_tbl_lleg) / sizeof(INTERPOLATION_DATA); break;
        case 4: part.tblMax = sizeof(idle_tbl_rleg) / sizeof(INTERPOLATION_DATA); break;
        default: part.tblMax = 0; break;
        }
    }
}

void UpdatePlayerAnimation(PLAYER* player)
{
    if (!player) return;

    INTERPOLATION_DATA** currentTblAdr = (currentAnim == ANIM_WALK) ? g_MoveTblAdr : g_IdleTblAdr;

    for (int i = 0; i < PLAYER_PARTS_MAX; i++)
    {
        PLAYER_PART& part = player->parts[i];
        if (!part.use || part.tblMax == 0) continue;

        int nowNo = (int)part.time;
        int maxNo = part.tblMax;
        int nextNo = (nowNo + 1) % maxNo;
        INTERPOLATION_DATA* tbl = currentTblAdr[part.tblNo];

        XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);
        XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);
        XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);

        XMVECTOR Pos = XMVectorSubtract(XMLoadFloat3(&tbl[nextNo].pos), nowPos);
        XMVECTOR Rot = XMVectorSubtract(XMLoadFloat3(&tbl[nextNo].rot), nowRot);
        XMVECTOR Scl = XMVectorSubtract(XMLoadFloat3(&tbl[nextNo].scl), nowScl);

        float nowTime = part.time - nowNo;

        Pos = XMVectorScale(Pos, nowTime);
        Rot = XMVectorScale(Rot, nowTime);
        Scl = XMVectorScale(Scl, nowTime);

        XMStoreFloat3(&part.pos, XMVectorAdd(nowPos, Pos));
        XMStoreFloat3(&part.rot, XMVectorAdd(nowRot, Rot));
        XMStoreFloat3(&part.scl, XMVectorAdd(nowScl, Scl));

        part.time += 1.0f / tbl[nowNo].frame;
        if ((int)part.time >= maxNo)
            part.time -= maxNo;
    }
}

void SetPlayerAnimation(PLAYER* player, PLAYER_ANIM anim)
{
    if (!player) return;

    if (currentAnim != anim)
    {
        currentAnim = anim;

        for (int i = 0; i < PLAYER_PARTS_MAX; i++)
        {
            PLAYER_PART& part = player->parts[i];
            part.time = 0.0f;
            part.tblNo = i;

            if (anim == ANIM_IDLE)
            {
                switch (i)
                {
                case 0: part.tblMax = sizeof(idle_tbl_head) / sizeof(INTERPOLATION_DATA); break;
                case 1: part.tblMax = sizeof(idle_tbl_larm) / sizeof(INTERPOLATION_DATA); break;
                case 2: part.tblMax = sizeof(idle_tbl_rarm) / sizeof(INTERPOLATION_DATA); break;
                case 3: part.tblMax = sizeof(idle_tbl_lleg) / sizeof(INTERPOLATION_DATA); break;
                case 4: part.tblMax = sizeof(idle_tbl_rleg) / sizeof(INTERPOLATION_DATA); break;
                default: part.tblMax = 0; break;
                }
            }
            else if (anim == ANIM_WALK)
            {
                switch (i)
                {
                case 0: part.tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA); break;
                case 1: part.tblMax = sizeof(move_tbl_larm) / sizeof(INTERPOLATION_DATA); break;
                case 2: part.tblMax = sizeof(move_tbl_rarm) / sizeof(INTERPOLATION_DATA); break;
                case 3: part.tblMax = sizeof(move_tbl_lleg) / sizeof(INTERPOLATION_DATA); break;
                case 4: part.tblMax = sizeof(move_tbl_rleg) / sizeof(INTERPOLATION_DATA); break;
                default: part.tblMax = 0; break;
                }
            }
        }
    }
}
