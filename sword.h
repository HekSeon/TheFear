//=============================================================================
//
// ���f������ [player.h]
// Author : 
//
//=============================================================================
#pragma once

#include "model.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define	SWORD_SIZE		(5.0f)				// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct SWORD
{
	XMFLOAT3		pos;		// �|���S���̈ʒu
	XMFLOAT3		oldpos;
	XMFLOAT3		rot;		// �|���S���̌���(��])
	XMFLOAT3		scl;		// �|���S���̑傫��(�X�P�[��)

	XMFLOAT4X4		mtxWorld;	// ���[���h�}�g���b�N�X

	BOOL			load;
	DX11_MODEL		model;		// ���f�����

	float			dir;		// ����
	float			size;		// �����蔻��̑傫��
	int				shadowIdx;	// �e��Index
	BOOL			use;

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	float			time;		// ���`��ԗp
	int				tblNo;		// �s���f�[�^�̃e�[�u���ԍ�
	int				tblMax;		// ���̃e�[�u���̃f�[�^��

	// �e�́ANULL�A�q���͐e�̃A�h���X������(�Z�[�u�����[�h�̎��́������C�����鎖)

	
};




//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSword(void);
void UninitSword(void);
void UpdateSword(void);
void DrawSword(void);

SWORD* GetSword(void);

void SetSwordPos(XMFLOAT3 offset, float rotX, float rotY, float rotZ);

void AttackNearestEnemy();

