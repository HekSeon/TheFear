#include "main.h"
#include <algorithm>
#include "enemy.h"
#include "collision.h"
#include "meshfield.h"
#include "player.h"
#include "score.h"
#include <initializer_list>

namespace
{
constexpr float EnemySpeed = 1.15f;
constexpr float EnemyGroundOffset = 16.065f;
constexpr float PlayerRadius = 6.0f;
constexpr int FirstWaveSize = 4;
constexpr int MaxWaveSize = 24;
constexpr int WaveRest = 150;
constexpr int AttackImpact = 19;
constexpr int AttackDuration = 48;

struct BearPart
{
    unsigned firstSubset;
    unsigned subsetCount;
    XMFLOAT3 pivot;
    float walkSwing;
    float attackSwing;
};

// Material subset order in data/MODEL/enemy/bear.obj. The face and ears
// occupy two ranges because the body and arms appear between them.
const BearPart parts[] = {
    {0, 9, {0, -2, 0}, 0.04f, 0.20f},  // Head, face, and first ear
    {9, 1, {}, 0.0f, 0.12f},            // Torso
    {10, 1, {11, -3.5f, 0}, 0.28f, 0.20f},
    {11, 1, {-11, -3.5f, 0}, -0.28f, -1.3f},
    {12, 4, {0, -2, 0}, 0.04f, 0.20f}, // Other eye and ear
    {16, 1, {0, -17, -8}, 0.10f, 0.0f}, // Tail
    {17, 1, {5, -23.5f, 0}, -0.35f, 0.0f},
    {18, 1, {-5, -23.5f, 0}, 0.35f, 0.0f},
};

DX11_MODEL bearModel = {};
bool modelsLoaded = false;
ENEMY enemies[MAX_ENEMY] = {};
int wave = 0;
int alive = 0;
int restFrames = 0;

void StartWave(const XMFLOAT3& center)
{
    const int count = FirstWaveSize + std::min(wave, (MaxWaveSize - FirstWaveSize) / 2) * 2;
    ++wave;
    const float phase = XM_2PI * (rand() / (RAND_MAX + 1.0f));
    for (int i = 0; i < count; ++i)
    {
        const float angle = phase + XM_2PI * i / count;
        // Try both sides of the player near the map edge.
        XMFLOAT3 ground;
        XMFLOAT3 normal;
        for (float radius : {240.0f, -240.0f, 120.0f, -120.0f})
        {
            XMFLOAT3 spawn = {center.x + sinf(angle) * radius, center.y,
                             center.z + cosf(angle) * radius};
            if (!RayHitField(spawn, &ground, &normal)) continue;
            ENEMY& enemy = enemies[alive++];
            enemy = {};
            enemy.pos = {ground.x, ground.y + EnemyGroundOffset, ground.z};
            enemy.rot.y = atan2f(center.x - ground.x, center.z - ground.z);
            enemy.health = 100.0f;
            enemy.use = TRUE;
            break;
        }
    }
    restFrames = WaveRest;
}
} // namespace

HRESULT InitEnemy(void)
{
    LoadModel("data/MODEL/enemy/bear.obj", &bearModel);
    modelsLoaded = true;
    for (ENEMY& enemy : enemies)
        enemy = {};
    wave = alive = 0;
    StartWave(GetPlayer()->pos);
    return S_OK;
}

void UninitEnemy(void)
{
    if (!modelsLoaded) return;
    UnloadModel(&bearModel);
    modelsLoaded = false;
}

XMFLOAT3 GetEnemyAttackCenter(const ENEMY& enemy)
{
    return {enemy.pos.x + sinf(enemy.rot.y) * EnemyAttackOffset, enemy.pos.y + 2.0f,
            enemy.pos.z + cosf(enemy.rot.y) * EnemyAttackOffset};
}

void DamageEnemy(ENEMY& enemy, float damage)
{
    if (!enemy.use) return;
    enemy.health = std::max(0.0f, enemy.health - damage);
    if (enemy.health > 0.0f) return;
    enemy.use = FALSE;
    --alive;
    AddScore(100);
}

void UpdateEnemy(void)
{
    PLAYER& player = *GetPlayer();
    if (!player.use) return;
    if (alive == 0)
    {
        if (--restFrames <= 0)
            StartWave(player.pos);
        return;
    }

    for (ENEMY& enemy : enemies)
    {
        if (!enemy.use) continue;
        const float dx = player.pos.x - enemy.pos.x;
        const float dz = player.pos.z - enemy.pos.z;
        const float distance = sqrtf(dx * dx + dz * dz);
        if (distance > 0.01f && enemy.attackFrame < AttackImpact)
            enemy.rot.y = atan2f(dx, dz);

        if (enemy.attackFrame > 0)
        {
            if (++enemy.attackFrame == AttackImpact &&
                CollisionBC(GetEnemyAttackCenter(enemy), player.pos, EnemyAttackRadius, PlayerRadius))
                player.health = std::max(0.0f, player.health - 10.0f);

            if (enemy.attackFrame >= AttackDuration)
                enemy.attackFrame = 0;
            continue;
        }

        if (distance <= EnemyAttackOffset + EnemyAttackRadius)
        {
            enemy.attackFrame = 1;
            continue;
        }

        XMFLOAT3 next = {enemy.pos.x + dx / distance * EnemySpeed, enemy.pos.y,
                         enemy.pos.z + dz / distance * EnemySpeed};
        XMFLOAT3 ground;
        XMFLOAT3 normal;
        if (!RayHitField(next, &ground, &normal)) continue;
        next.y = ground.y + EnemyGroundOffset;

        bool blocked = false;
        for (const ENEMY& other : enemies)
        {
            if (&other == &enemy || !other.use) continue;
            XMFLOAT3 separation = {other.pos.x, next.y, other.pos.z};
            if (CollisionBC(next, separation, EnemyBodyRadius, EnemyBodyRadius))
            {
                blocked = true;
                break;
            }
        }
        if (blocked) continue;
        enemy.pos = next;
        enemy.walkPhase = fmodf(enemy.walkPhase + 0.2f, XM_2PI);
    }
}

void DrawEnemy(void)
{
    SetCullingMode(CULL_MODE_NONE);
    for (const ENEMY& enemy : enemies)
    {
        if (!enemy.use) continue;
        const float walk = sinf(enemy.walkPhase);
        float strike = 0.0f;
        if (enemy.attackFrame > 0)
        {
            const float frame = static_cast<float>(enemy.attackFrame);
            if (frame < 12.0f)
                strike = -0.55f * frame / 12.0f;
            else if (frame <= AttackImpact)
                strike = -0.55f + 1.55f * (frame - 12.0f) / (AttackImpact - 12.0f);
            else
                strike = (AttackDuration - frame) / (AttackDuration - AttackImpact);
        }

        const XMMATRIX root = XMMatrixScaling(0.5f, 0.5f, 0.5f) *
            XMMatrixRotationY(enemy.rot.y) *
            XMMatrixTranslation(enemy.pos.x, enemy.pos.y, enemy.pos.z);
        for (const BearPart& part : parts)
        {
            const float pitch = enemy.attackFrame > 0 ? strike * part.attackSwing : walk * part.walkSwing;
            const XMFLOAT3& p = part.pivot;
            XMMATRIX world = XMMatrixTranslation(-p.x, -p.y, -p.z) * XMMatrixRotationX(pitch) *
                XMMatrixTranslation(p.x, p.y, p.z) * root;
            SetWorldMatrix(&world);
            DrawModelSubsets(&bearModel, part.firstSubset, part.subsetCount);
        }
    }
    SetCullingMode(CULL_MODE_BACK);
}

const ENEMY* GetEnemy(void)
{
    return enemies;
}

ENEMY* FindNearestEnemy(XMFLOAT3 position)
{
    ENEMY* closest = nullptr;
    float bestDistance = 0.0f;
    for (ENEMY& enemy : enemies)
    {
        if (!enemy.use) continue;
        const float distance = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&position) - XMLoadFloat3(&enemy.pos)));
        if (closest && distance >= bestDistance) continue;
        bestDistance = distance;
        closest = &enemy;
    }
    return closest;
}

void DamageEnemiesInRange(XMFLOAT3 center, float radius, float damage)
{
    for (ENEMY& enemy : enemies)
        if (enemy.use && CollisionBC(center, enemy.pos, radius, EnemyBodyRadius))
            DamageEnemy(enemy, damage);
}

int GetEnemyWave(void) { return wave; }
int GetAliveEnemyCount(void) { return alive; }
int GetWaveRestFrames(void) { return restFrames; }
bool IsEnemyAttackActive(const ENEMY& enemy) { return enemy.attackFrame == AttackImpact; }
