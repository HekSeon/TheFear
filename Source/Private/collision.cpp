///////////////////////////////////
// 
// collision.cpp
// 
// Author: BAYAR "HekSeon" SEMIH
///////////////////////////////////
#include "main.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義 (Macros)
//*****************************************************************************

//*****************************************************************************
// 構造体定義 (Structures)
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言 (Prototypes)
//*****************************************************************************

//*****************************************************************************
// グローバル変数 (Global Variables)
//*****************************************************************************

//=============================================================================
// BB (Bounding Box) による当たり判定処理
// 回転は考慮しない
// 戻り値：当たっていたらTRUE
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh, XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// 初期値はFALSE (当たっていない) に設定

	// 座標が中心点なので計算しやすいように半分にする
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// バウンディングボックス(BB)の判定処理
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
// BC (Bounding Circle) による当たり判定処理
// サイズは半径
// 戻り値：当たっていたらTRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// 初期値はFALSEに設定

	float len = (r1 + r2) * (r1 + r2);		// 半径の合計を2乗したもの
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2点間の距離の2乗
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// 半径の2乗よりも距離の2乗が短ければ衝突している
	if (len > lenSq)
	{
		ans = TRUE;	// 当たっている
	}

	return ans;
}

//=============================================================================
// 内積 (Dot Product)
//=============================================================================
float dotProduct(XMVECTOR* v1, XMVECTOR* v2)
{
#if 0
	float ans = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
#else
	// DirectXMathを使用した処理
	XMVECTOR temp = XMVector3Dot(*v1, *v2);
	float ans = 0.0f;
	XMStoreFloat(&ans, temp);
#endif

	return(ans);
}

//=============================================================================
// 外積 (Cross Product)
//=============================================================================
void crossProduct(XMVECTOR* ret, XMVECTOR* v1, XMVECTOR* v2)
{
#if 0
	ret->x = v1->y * v2->z - v1->z * v2->y;
	ret->y = v1->z * v2->x - v1->x * v2->z;
	ret->z = v1->x * v2->y - v1->y * v2->x;
#else
	// DirectXMathを使用した処理
	* ret = XMVector3Cross(*v1, *v2);
#endif
}

//=============================================================================
// レイキャスト (RayCast)
// p0, p1, p2 : ポリゴンの3頂点
// pos0       : 始点
// pos1       : 終点
// hit        : 交点の返却用バッファ
// normal     : 法線ベクトルの返却用バッファ
// 戻り値     : 当たっている場合はTRUEを返す
//=============================================================================
BOOL RayCast(XMFLOAT3 xp0, XMFLOAT3 xp1, XMFLOAT3 xp2, XMFLOAT3 xpos0, XMFLOAT3 xpos1, XMFLOAT3* hit, XMFLOAT3* normal)
{
	XMVECTOR	p0 = XMLoadFloat3(&xp0);
	XMVECTOR	p1 = XMLoadFloat3(&xp1);
	XMVECTOR	p2 = XMLoadFloat3(&xp2);
	XMVECTOR	pos0 = XMLoadFloat3(&xpos0);
	XMVECTOR	pos1 = XMLoadFloat3(&xpos1);

	XMVECTOR	nor;	// ポリゴンの法線
	XMVECTOR	vec1;
	XMVECTOR	vec2;
	float		d1, d2;

	{	// ポリゴンの外積をとって法線を求める (固定オブジェクトなら本来はInit等で予め計算しておくと高効率)
		vec1 = p1 - p0;
		vec2 = p2 - p0;
		crossProduct(&nor, &vec2, &vec1);
		nor = XMVector3Normalize(nor);		// 計算しやすいように法線を正規化 (長さを1にする)
		XMStoreFloat3(normal, nor);			// 求めた法線を格納
	}

	// ポリゴン平面と線分の内積をとって衝突の可能性を調べる (鋭角なら+, 鈍角なら-, 直角なら0)
	vec1 = pos0 - p0;
	vec2 = pos1 - p0;
	{	// 法線と2つのベクトル (線分の両端からポリゴン上の点へのベクトル) の内積で跨いでいるかチェック
		d1 = dotProduct(&vec1, &nor);
		d2 = dotProduct(&vec2, &nor);
		if (((d1 * d2) > 0.0f) || (d1 == 0 && d2 == 0))
		{
			// 面の同じ側にあるか、完全に平行な場合は交差の可能性なし
			return(FALSE);
		}
	}

	{	// ポリゴン平面と線分の交点を求める
		d1 = (float)fabs(d1);	// 絶対値を求める
		d2 = (float)fabs(d2);	// 絶対値を求める
		float a = d1 / (d1 + d2);							// 内分比

		XMVECTOR	vec3 = (1 - a) * vec1 + a * vec2;		// p0から交点へのベクトル
		XMVECTOR	p3 = p0 + vec3;							// 交点座標
		XMStoreFloat3(hit, p3);								// 交点を格納

		{	// 求めた交点が三角形の内部にあるかを調べる

			// ポリゴンの各辺のベクトル
			XMVECTOR	v1 = p1 - p0;
			XMVECTOR	v2 = p2 - p1;
			XMVECTOR	v3 = p0 - p2;

			// 各頂点から交点へのベクトル
			XMVECTOR	v4 = p3 - p1;
			XMVECTOR	v5 = p3 - p2;
			XMVECTOR	v6 = p3 - p0;

			// 外積で各辺の法線を求め、ポリゴン法線との向きの一致 (内積の符号) をチェック
			XMVECTOR	n1, n2, n3;

			crossProduct(&n1, &v4, &v1);
			if (dotProduct(&n1, &nor) < 0.0f) return(FALSE);	// 外側のため衝突なし

			crossProduct(&n2, &v5, &v2);
			if (dotProduct(&n2, &nor) < 0.0f) return(FALSE);	// 外側のため衝突なし

			crossProduct(&n3, &v6, &v3);
			if (dotProduct(&n3, &nor) < 0.0f) return(FALSE);	// 外側のため衝突なし
		}
	}

	return(TRUE);	// 衝突判定成立 (hitに交点、normalに法線が正しく代入されています)
}