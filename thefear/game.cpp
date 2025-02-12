//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"

#include "game.h"
#include "player.h"

#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
//void CheckHit(void);



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// �|�[�YON/OFF


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	InitPlayer();

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	
	//UninitParticle();
	UninitPlayer();
	

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
	UpdatePlayer();

#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}

#endif

	if (g_bPause == FALSE)
		return;



	//CheckHit();

	
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	
	PLAYER* g_player = GetPlayer();
	XMFLOAT3 pos = g_player[0].pos;
	SetCameraAT(pos);
	SetCamera();
	SetFogEnable(TRUE);

	DrawPlayer();
	
	SetDepthEnable(FALSE);

	
	SetLightEnable(FALSE);



//3d yapar
	SetLightEnable(TRUE);

	SetDepthEnable(TRUE);

	SetFogEnable(TRUE);
}


void DrawGame(void)
{
#ifdef _DEBUG
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif


	switch (g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);

		DrawGame0();
	
	
	
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
//void CheckHit(void)
//{
//	ENEMY* enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
//	PLAYER* player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
//	BULLET* bullet = GetBullet();	// �e�̃|�C���^�[��������
//
//	// �G�ƃv���C���[�L����
//	for (int i = 0; i < MAX_ENEMY; i++)
//	{
//		//�G�̗L���t���O���`�F�b�N����
//		if (enemy[i].use == FALSE)
//			continue;
//
//		//BC�̓����蔻��
//		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
//		{
//			// �G�L�����N�^�[�͓|�����
//			enemy[i].use = FALSE;
//			ReleaseShadow(enemy[i].shadowIdx);
//
//			// �X�R�A�𑫂�
//			AddScore(100);
//		}
//	}
//
//
//	// �v���C���[�̒e�ƓG
//	for (int i = 0; i < MAX_BULLET; i++)
//	{
//		//�e�̗L���t���O���`�F�b�N����
//		if (bullet[i].use == FALSE)
//			continue;
//
//		// �G�Ɠ������Ă邩���ׂ�
//		for (int j = 0; j < MAX_ENEMY; j++)
//		{
//			//�G�̗L���t���O���`�F�b�N����
//			if (enemy[j].use == FALSE)
//				continue;
//
//			//BC�̓����蔻��
//			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
//			{
//				// �����������疢�g�p�ɖ߂�
//				bullet[i].use = FALSE;
//				ReleaseShadow(bullet[i].shadowIdx);
//
//				// �G�L�����N�^�[�͓|�����
//				enemy[j].use = FALSE;
//				ReleaseShadow(enemy[j].shadowIdx);
//
//				// �X�R�A�𑫂�
//				AddScore(10);
//			}
//		}
//
//	}
//
//
//	// �G�l�~�[���S�����S�������ԑJ��
//	int enemy_count = 0;
//	for (int i = 0; i < MAX_ENEMY; i++)
//	{
//		if (enemy[i].use == FALSE) continue;
//		enemy_count++;
//	}
//
//	// �G�l�~�[���O�C�H
//	if (enemy_count == 0)
//	{
//		SetFade(FADE_OUT, MODE_RESULT);
//	}
//
//}


