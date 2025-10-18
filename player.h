#pragma once
#include "model.h"

#define MAX_PLAYER        (1)
#define PLAYER_SIZE       (5.0f)
#define MODEL_PLAYER      "data/MODEL/bear/body.obj"
#define MODEL_PLAYER_HEAD "data/MODEL/bear/head.obj"
#define MODEL_PLAYER_LARM "data/MODEL/bear/leftarm.obj"
#define MODEL_PLAYER_RARM "data/MODEL/bear/rightarm.obj"
#define MODEL_PLAYER_LLEG "data/MODEL/bear/leftleg.obj"
#define MODEL_PLAYER_RLEG "data/MODEL/bear/rightleg.obj"
#define PLAYER_PARTS_MAX  (5)

struct INTERPOLATION_DATA; // forward declaration
struct PLAYER;              // forward declaration, PLAYER_PART içinde pointer kullanmak için

struct PLAYER_PART
{
    DX11_MODEL model;
    XMFLOAT3 pos;
    XMFLOAT3 rot;
    XMFLOAT3 scl;
    XMFLOAT4X4 mtxWorld;
    float time;
    int tblNo;
    int tblMax;
    INTERPOLATION_DATA* tbl;
    BOOL use;
    BOOL load;
    PLAYER* parent; // pointer olduğu için forward declaration yeterli
};

struct PLAYER
{
    XMFLOAT3 pos;
    XMFLOAT3 oldpos;
    XMFLOAT3 rot;
    XMFLOAT3 scl;
    XMFLOAT4X4 mtxWorld;

    BOOL load;
    DX11_MODEL model;
    float spd;
    float dir;
    float size;
	float health;
    int shadowIdx;
    BOOL use;
    BOOL move;
    BOOL attack;

    PLAYER_PART parts[PLAYER_PARTS_MAX];

    int currentTblAdr;
    XMFLOAT4 Quaternion;
    XMFLOAT3 UpVector;
};

// Prototipler
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
PLAYER* GetPlayer(void);
