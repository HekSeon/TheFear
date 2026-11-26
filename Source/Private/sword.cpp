#include "sword.h"

#include <algorithm>
#include "collision.h"
#include "debugproc.h"
#include "enemy.h"
#include "input.h"
#include "meshfield.h"
#include "player.h"

namespace
{
constexpr float FlightSpeed = 6.0f;
constexpr float ImpactDamage = 50.0f;
constexpr float ScatterSpeed = 0.035f;
constexpr float AssembleSpeed = 0.04f;
constexpr float BackScale = 0.055f;
constexpr float FlightScale = 0.1f;
constexpr float BladeTipLocalZ = -303.4f;
constexpr float HoverStep = XM_2PI / 150.0f;

enum class SwordState { Following, Flying, Scattering, Assembling };

SWORD sword = {};
SwordState state = SwordState::Following;
ENEMY* targetEnemy = nullptr;
float hoverPhase = 0.0f;

void FollowPlayer()
{
    const PLAYER& player = *GetPlayer();
    const float yaw = player.rot.y + XM_PI;
    const XMVECTOR offset = XMVector3TransformNormal(
        XMVectorSet(3.0f, 6.0f, -8.0f + 0.6f * cosf(hoverPhase), 0.0f),
        XMMatrixRotationY(yaw));
    XMStoreFloat3(&sword.pos, XMLoadFloat3(&player.pos) + offset);

    // sword.obj points along local -Z. Keep its tip above any uphill ground behind the player.
    sword.rot = {-XM_PIDIV2, yaw, 0.0f};
    sword.scl = {BackScale, BackScale, BackScale};
    XMFLOAT3 ground, normal;
    if (RayHitField(sword.pos, &ground, &normal))
        sword.pos.y = std::max(sword.pos.y, ground.y - BladeTipLocalZ * BackScale + 2.0f);
    sword.pos.y += 1.25f * sinf(hoverPhase);
}

void BeginAssembly()
{
    targetEnemy = nullptr;
    FollowPlayer();
    StartDissolveIn(&sword.dissolve, AssembleSpeed, 0.0f, 1.0f);
    state = SwordState::Assembling;
}
}

HRESULT InitSword(void)
{
    LoadModel("data/MODEL/sword.obj", &sword.model);
    sword.load = TRUE;
    sword.use = TRUE;
    sword.size = SWORD_SIZE;
    sword.dissolve = {};
    state = SwordState::Following;
    targetEnemy = nullptr;
    hoverPhase = 0.0f;
    FollowPlayer();
    return S_OK;
}

void UninitSword(void)
{
    if (sword.load)
        UnloadModel(&sword.model);
    sword = {};
    targetEnemy = nullptr;
    state = SwordState::Following;
}

void UpdateSword(void)
{
    hoverPhase = fmodf(hoverPhase + HoverStep, XM_2PI);
    switch (state)
    {
    case SwordState::Following:
        FollowPlayer();
        if (!IsGameDebugOpen() && GetKeyboardTrigger(DIK_SPACE))
            AttackNearestEnemy();
        break;

    case SwordState::Flying:
    {
        if (!targetEnemy || !targetEnemy->use)
            targetEnemy = FindNearestEnemy(sword.pos);
        if (!targetEnemy)
        {
            BeginAssembly();
            break;
        }

        const float scale = std::min(sword.scl.x + 0.006f, FlightScale);
        sword.scl = {scale, scale, scale};

        const XMFLOAT3 target = targetEnemy->pos;
        const XMVECTOR position = XMLoadFloat3(&sword.pos);
        const XMVECTOR delta = XMLoadFloat3(&target) - position;
        const float distance = XMVectorGetX(XMVector3Length(delta));
        if (distance > 0.001f)
        {
            const float dx = target.x - sword.pos.x;
            const float dy = target.y - sword.pos.y;
            const float dz = target.z - sword.pos.z;
            sword.rot = {atan2f(dy, sqrtf(dx * dx + dz * dz)), atan2f(dx, dz) + XM_PI, 0.0f};

            // Stop at the first contact along this frame's flight step.
            const float contact = distance - sword.size - EnemyBodyRadius;
            const float step = std::clamp(contact + 0.01f, 0.0f, FlightSpeed);
            XMStoreFloat3(&sword.pos, position + delta * (step / distance));
        }
        if (CollisionBC(sword.pos, target, sword.size, EnemyBodyRadius))
        {
            DamageEnemy(*targetEnemy, ImpactDamage);
            targetEnemy = nullptr;
            StartDissolveOut(&sword.dissolve, ScatterSpeed, 0.0f, 1.0f);
            state = SwordState::Scattering;
        }
        break;
    }

    case SwordState::Scattering:
        if (bUpdateDdissolve(&sword.dissolve, 1.0f))
            BeginAssembly();
        break;

    case SwordState::Assembling:
        FollowPlayer();
        if (bUpdateDdissolve(&sword.dissolve, 1.0f))
            state = SwordState::Following;
        break;
    }
}

void AttackNearestEnemy()
{
    if (state != SwordState::Following || !GetPlayer()->use) return;
    targetEnemy = FindNearestEnemy(GetPlayer()->pos);
    if (targetEnemy)
        state = SwordState::Flying;
}

void DrawSword(void)
{
    if (!sword.use) return;

    XMMATRIX world = XMMatrixScaling(sword.scl.x, sword.scl.y, sword.scl.z) *
        XMMatrixRotationRollPitchYaw(sword.rot.x, sword.rot.y, sword.rot.z) *
        XMMatrixTranslation(sword.pos.x, sword.pos.y, sword.pos.z);
    SetWorldMatrix(&world);
    XMStoreFloat4x4(&sword.mtxWorld, world);
    SetCullingMode(CULL_MODE_NONE);
    const bool fragments = sword.dissolve.mode != DISSOLVE_NONE;
    if (fragments) SetSwordFragments(true, sword.dissolve.threshold);
    DrawModel(&sword.model);
    if (fragments) SetSwordFragments(false, 0.0f);
    SetCullingMode(CULL_MODE_BACK);
}

SWORD* GetSword(void)
{
    return &sword;
}
