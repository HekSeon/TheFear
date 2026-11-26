#pragma once

#include "main.h"

constexpr int MAX_ENEMY = 100;
constexpr float EnemyBodyRadius = 4.5f;
constexpr float EnemyAttackOffset = 13.0f;
constexpr float EnemyAttackRadius = 9.0f;

struct ENEMY
{
    XMFLOAT3 pos;
    XMFLOAT3 rot;
    BOOL use;
    float health;
    float walkPhase;
    int attackFrame;
};

HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

const ENEMY* GetEnemy(void);
ENEMY* FindNearestEnemy(XMFLOAT3 position);
void DamageEnemy(ENEMY& enemy, float damage);
void DamageEnemiesInRange(XMFLOAT3 center, float radius, float damage);
XMFLOAT3 GetEnemyAttackCenter(const ENEMY& enemy);
bool IsEnemyAttackActive(const ENEMY& enemy);
int GetEnemyWave(void);
int GetAliveEnemyCount(void);
int GetWaveRestFrames(void);
