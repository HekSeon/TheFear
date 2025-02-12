//=============================================================================
//
// 当たり判定処理 [collision.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "collision.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************


//=============================================================================
// BBによる当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらTRUE
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// 外れをセットしておく

	// 座標が中心点なので計算しやすく半分にしている
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// バウンディングボックス(BB)の処理
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// 当たった時の処理
		ans = TRUE;
	}

	return ans;
}

//=============================================================================
// BCによる当たり判定処理
// サイズは半径
// 戻り値：当たってたらTRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// 外れをセットしておく

	float len = (r1 + r2) * (r1 + r2);		// 半径を2乗した物
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2点間の距離（2乗した物）
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// 半径を2乗した物より距離が短い？
	if (len > lenSq)
	{
		ans = TRUE;	// 当たっている
	}

	return ans;
}


//=============================================================================
// 内積(dot)
//=============================================================================
float dotProduct(XMVECTOR *v1, XMVECTOR *v2)
{
#if 0
	float ans = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
#else
	// ダイレクトＸでは、、、
	XMVECTOR temp = XMVector3Dot(*v1, *v2);
	float ans = 0.0f;
	XMStoreFloat(&ans, temp);
#endif

	return(ans);
}


//=============================================================================
// 外積(cross)
//=============================================================================
void crossProduct(XMVECTOR *ret, XMVECTOR *v1, XMVECTOR *v2)
{
#if 0
	ret->x = v1->y * v2->z - v1->z * v2->y;
	ret->y = v1->z * v2->x - v1->x * v2->z;
	ret->z = v1->x * v2->y - v1->y * v2->x;
#else
	// ダイレクトＸでは、、、
	*ret = XMVector3Cross(*v1, *v2);
#endif

}


//=============================================================================
// レイキャスト
// p0, p1, p2　ポリゴンの３頂点
// pos0 始点
// pos1 終点
// hit　交点の返却用
// normal 法線ベクトルの返却用
// 当たっている場合、TRUEを返す
//=============================================================================
BOOL RayCast(XMFLOAT3 xp0, XMFLOAT3 xp1, XMFLOAT3 xp2, XMFLOAT3 xpos0, XMFLOAT3 xpos1, XMFLOAT3 *hit, XMFLOAT3 *normal)
{
	XMVECTOR	p0   = XMLoadFloat3(&xp0);
	XMVECTOR	p1   = XMLoadFloat3(&xp1);
	XMVECTOR	p2   = XMLoadFloat3(&xp2);
	XMVECTOR	pos0 = XMLoadFloat3(&xpos0);
	XMVECTOR	pos1 = XMLoadFloat3(&xpos1);

	XMVECTOR	nor;	// ポリゴンの法線
	XMVECTOR	vec1;
	XMVECTOR	vec2;
	float		d1, d2;

	{	// ポリゴンの外積をとって法線を求める(この処理は固定物なら予めInit()で行っておくと良い)
		vec1 = p1 - p0;
		vec2 = p2 - p0;
		crossProduct(&nor, &vec2, &vec1);
		nor = XMVector3Normalize(nor);		// 計算しやすいように法線をノーマライズしておく(このベクトルの長さを１にしている)
		XMStoreFloat3(normal, nor);			// 求めた法線を入れておく
	}

	// ポリゴン平面と線分の内積とって衝突している可能性を調べる（鋭角なら＋、鈍角ならー、直角なら０）
	vec1 = pos0 - p0;
	vec2 = pos1 - p0;
	{	// 求めたポリゴンの法線と２つのベクトル（線分の両端とポリゴン上の任意の点）の内積とって衝突している可能性を調べる
		d1 = dotProduct(&vec1, &nor);
		d2 = dotProduct(&vec2, &nor);
		if (((d1 * d2) > 0.0f) || (d1 == 0 && d2 == 0))
		{
			// 当たっている可能性は無い
			return(FALSE);
		}
	}


	{	// ポリゴンと線分の交点を求める
		d1 = (float)fabs(d1);	// 絶対値を求めている
		d2 = (float)fabs(d2);	// 絶対値を求めている
		float a = d1 / (d1 + d2);							// 内分比

		XMVECTOR	vec3 = (1 - a) * vec1 + a * vec2;		// p0から交点へのベクトル
		XMVECTOR	p3 = p0 + vec3;							// 交点
		XMStoreFloat3(hit, p3);								// 求めた交点を入れておく

		{	// 求めた交点がポリゴンの中にあるか調べる

			// ポリゴンの各辺のベクトル
			XMVECTOR	v1 = p1 - p0;
			XMVECTOR	v2 = p2 - p1;
			XMVECTOR	v3 = p0 - p2;

			// 各頂点と交点とのベクトル
			XMVECTOR	v4 = p3 - p1;
			XMVECTOR	v5 = p3 - p2;
			XMVECTOR	v6 = p3 - p0;

			// 外積で各辺の法線を求めて、ポリゴンの法線との内積をとって符号をチェックする
			XMVECTOR	n1, n2, n3;

			crossProduct(&n1, &v4, &v1);
			if (dotProduct(&n1, &nor) < 0.0f) return(FALSE);	// 当たっていない

			crossProduct(&n2, &v5, &v2);
			if (dotProduct(&n2, &nor) < 0.0f) return(FALSE);	// 当たっていない
			
			crossProduct(&n3, &v6, &v3);
			if (dotProduct(&n3, &nor) < 0.0f) return(FALSE);	// 当たっていない
		}
	}

	return(TRUE);	// 当たっている！(hitには当たっている交点が入っている。normalには法線が入っている)
}



