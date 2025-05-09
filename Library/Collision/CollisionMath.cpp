#include "CollisionMath.h"

namespace CollisionHelper
{
	// 外部の点に対する三角形内部の最近点を取得する
	static bool GetClosestPoint_PointTriangle(const DirectX::XMVECTOR& point,
		const DirectX::XMVECTOR trianglePos[3],
		DirectX::XMVECTOR& nearPos)
	{
		using namespace DirectX;
		// pointがTrianglePos[0]の外側の頂点領域にあるかチェック
		DirectX::XMVECTOR Vec01 = trianglePos[1] - trianglePos[0];// 青
		DirectX::XMVECTOR Vec02 = trianglePos[2] - trianglePos[0];// 緑
		DirectX::XMVECTOR Vec0P = point - trianglePos[0];// 黒
		float d1 = XMVectorGetX(XMVector3Dot(Vec01, Vec0P));// 青・黒
		float d2 = XMVectorGetX(XMVector3Dot(Vec02, Vec0P));// 緑・黒
		// 内積の値が負<=>角度が90度を超えている
		if (d1 < 0.0f || d2 < 0.0f)
		{
			// TrianglePos[0]が最近点
			nearPos = trianglePos[0];
			return false;
		}


		// pointがTrianglePos[1]の外側の頂点領域にあるかチェック
		DirectX::XMVECTOR Vec1P = point - trianglePos[1];// 赤
		float d3 = XMVectorGetX(XMVector3Dot(Vec01, Vec1P));// 青・赤
		float d4 = XMVectorGetX(XMVector3Dot(Vec02, Vec1P));// 緑・赤
		if (d3 < 0.0f)
			return false;
		// 橙・赤 < 0
		// (緑-青) ・ 赤 < 0
		// 緑・赤 < 青・赤
		if (d4 < d3)
		{
			// TrianglePos[1]が最近点
			nearPos = trianglePos[1];
			return false;
		}

		// pointがTrianglePos[0]とTrianglePos[1]の外側の辺領域にあるかチェック
		// v = Dot(Cross(AP,BP), Cross(AB,AC))
		// ラグランジュの恒等式より
		// v = Dot(AP,AB)Dot(BP,AC) - Dot(AP,AC)Dot(BP,AB)
		// Dot(AP,AB) = d1 
		// Dot(BP,AC) = d4
		// Dot(AP,AC) = d3
		// Dot(BP,AB) = d2
		float v = d1 * d4 - d3 * d2;
		if (v < 0.0f)
		{
			// 辺AB上の最近点を求める
			float t = d1 / (d1 - d3);
			nearPos = XMVectorScale(trianglePos[0], t);
			return false;
		}

		// pointがTrianglePos[2]の外側の頂点領域にあるかチェック
		DirectX::XMVECTOR Vec2P = point - trianglePos[2];// 黄
		float d5 = XMVectorGetX(XMVector3Dot(Vec01, Vec2P));// 青・黄
		float d6 = XMVectorGetX(XMVector3Dot(Vec02, Vec2P));// 緑・黄
		if (d1 < 0.0f || d2 < 0.0f)
		{
			// TrianglePos[2]が最近点
			nearPos = trianglePos[2];
			return false;
		}

		// pointがTrianglePos[0]とTrianglePos[2]の外側の辺領域にあるかチェック
		// u = Dot(Cross(CP,AP), Cross(AB,AC))
		// ラグランジュの恒等式より
		// u = Dot(CP,AB)Dot(AP,AC) - Dot(CP,AC)Dot(AP,AB)
		// Dot(CP,AB) = d5 
		// Dot(AP,AC) = d2
		// Dot(CP,AC) = d1
		// Dot(AP,AB) = d6
		float u = d5 * d2 - d1 * d6;
		if (u < 0.0f)
		{
			// 辺AC上の最近点を求める
			float t = (d4 - d3) / ((d4 - d3) - (d6 - d5));
			nearPos = XMVectorScale(trianglePos[0], t);
			return false;
		}

		// pointがTrianglePos[1]とTrianglePos[2]の外側の辺領域にあるかチェック
		// w = Dot(Cross(BP,CP), Cross(AB,AC))
		// ラグランジュの恒等式より
		// w = Dot(BP,AB)Dot(CP,AC) - Dot(BP,AC)Dot(CP,AB)
		// Dot(BP,AB) = d3
		// Dot(CP,AC) = d6
		// Dot(BP,AC) = d5
		// Dot(CP,AB) = d4
		float w = d3 * d6 - d5 * d4;
		if (w < 0.0f)
		{
			// 辺BC上の最近点を求める
			float t = d2 / (d2 - d6);
			nearPos = XMVectorScale(trianglePos[0], t);
			return false;
		}

		// ここまでくれば、nearPosは三角形の内部にある
		float t01 = u / (w + u + v);
		float t02 = v / (w + u + v);
		nearPos = trianglePos[0] + XMVectorScale(Vec01, t01) + XMVectorScale(Vec02, t02);
		return true;
	}

	// スフィアキャストorカプセルVsAABB用の頂点算出関数
	inline DirectX::XMVECTOR GetAABBCorner(
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		const int v)
	{
		DirectX::XMFLOAT3 p = {};
		DirectX::XMStoreFloat3(&p, aabbPos);
		p.x += (v & (1 << 0)) ? DirectX::XMVectorGetX(aabbRadii) : -DirectX::XMVectorGetX(aabbRadii);
		p.y += (v & (1 << 1)) ? DirectX::XMVectorGetY(aabbRadii) : -DirectX::XMVectorGetY(aabbRadii);
		p.z += (v & (1 << 2)) ? DirectX::XMVectorGetZ(aabbRadii) : -DirectX::XMVectorGetZ(aabbRadii);

		return DirectX::XMLoadFloat3(&p);
	}

	// 線分と線分の最短距離の二乗を取得する
	float GetMinDistSq_SegmentSegment(const DirectX::XMVECTOR& point1A,
		const DirectX::XMVECTOR& point1B,
		const DirectX::XMVECTOR& point2A,
		const DirectX::XMVECTOR& point2B,
		DirectX::XMVECTOR* nearPoint1,
		DirectX::XMVECTOR* nearPoint2)
	{
		DirectX::XMVECTOR segmentDirection1 = DirectX::XMVectorSubtract(point1B, point1A);
		DirectX::XMVECTOR segmentDirection2 = DirectX::XMVectorSubtract(point2B, point2A);
		DirectX::XMVECTOR r = DirectX::XMVectorSubtract(point1A, point2A);

		float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection1));
		float e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, segmentDirection2));
		float f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, r));

		float t1 = 0.0f, t2 = 0.0f;

		if (a <= FLT_EPSILON && e <= FLT_EPSILON)	// 両線分が点に縮退している場合
		{
			t1 = t2 = 0.0f;
		}
		else if (a <= FLT_EPSILON)					// 片方（d0）が点に縮退している場合
		{
			t1 = 0.0f;
			t2 = std::clamp(f / e, 0.0f, 1.0f);
		}
		else
		{
			float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, r));
			if (e <= FLT_EPSILON)					// 片方（d1）が点に縮退している場合
			{
				t2 = 0.0f;
				t1 = std::clamp(-c / a, 0.0f, 1.0f);
			}
			else									// 両方が線分
			{
				float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection2));
				float denom = a * e - b * b;

				if (denom != 0.0f)					// 平行確認（平行時は t1 = 0.0f（線分の始端）を仮の初期値として計算をすすめる）
				{
					t1 = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
				}

				t2 = b * t1 + f;

				if (t2 < 0.0f)						// t1が始端より外側にある場合
				{
					t2 = 0.0f;
					t1 = std::clamp(-c / a, 0.0f, 1.0f);
				}
				else if (t2 > e)					// t1が終端より外側にある場合
				{
					t2 = 1.0f;
					t1 = std::clamp((b - c) / a, 0.0f, 1.0f);
				}
				else								// t1が線分上にある場合
				{
					t2 /= e;
				}
			}
		}

		// 各線分上の最近点算出
		DirectX::XMVECTOR point1 = DirectX::XMVectorAdd(point1A, DirectX::XMVectorScale(segmentDirection1, t1));
		DirectX::XMVECTOR point2 = DirectX::XMVectorAdd(point2A, DirectX::XMVectorScale(segmentDirection2, t2));

		DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);

		if (nearPoint1)
		{
			*nearPoint1 = point1;
		}
		if (nearPoint2)
		{
			*nearPoint2 = point2;
		}

		return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec));
	}

	// 線分とAABBの最短距離について、面領域に線分の端点がある場合の処理
	inline float GetMinDistSq_SegmentAABBFace_PointInFaceArea(
		const  DirectX::XMVECTOR& targetPoint,
		const  DirectX::XMVECTOR& otherPoint,
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		const int u,
		const int v,
		DirectX::XMVECTOR* nearPointSegment,
		DirectX::XMVECTOR* nearPointAABB)
	{
		DirectX::XMVECTOR nearTargetPoint = Collision3D::GetClosestPoint_PointAABB(targetPoint, aabbPos, aabbRadii);
		float minDist = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(targetPoint, nearTargetPoint)));
		if (nearPointSegment) *nearPointSegment = targetPoint;
		if (nearPointSegment) *nearPointAABB = nearTargetPoint;

		std::vector<DirectX::XMVECTOR> aabbFaceCorner;
		if (u == 1)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		}
		else if (u == 2)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		}
		else if (u == 4)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		}
		else if (v == 1)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		}
		else if (v == 2)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		}
		else if (v == 4)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		}


		DirectX::XMVECTOR tmpNearPointSegment = {};
		DirectX::XMVECTOR tmpNearPointAABB = {};
		float dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[1], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[2], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[3], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[0], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}

		return minDist;
	}

	// 線分とAABBの最短距離について、頂点領域に線分の端点がある場合の処理
	inline float GetMinDistSq_SegmentAABBFace_PointInCornerArea(
		const  DirectX::XMVECTOR& targetPoint,
		const  DirectX::XMVECTOR& otherPoint,
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		const int u,
		const int v,
		DirectX::XMVECTOR* nearPointSegment,
		DirectX::XMVECTOR* nearPointAABB)
	{
		std::vector<DirectX::XMVECTOR> aabbFaceCorner;
		if (u == 7 && v == 0)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		}
		else if (u == 6 && v == 1)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		}
		else if (u == 5 && v == 2)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		}
		else if (u == 4 && v == 3)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		}
		else if (u == 3 && v == 4)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		}
		else if (u == 2 && v == 5)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		}
		else if (u == 1 && v == 6)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		}
		else if (u == 0 && v == 7)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		}

		DirectX::XMVECTOR tmpNearPointSegment = {};
		DirectX::XMVECTOR tmpNearPointAABB = {};
		float minDist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[1], nearPointSegment, nearPointAABB);
		float dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[2], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[3], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[0], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[4], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[6], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[4], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[5], aabbFaceCorner[6], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}

		return minDist;
	}

	// 線分とAABBの最短距離について、辺領域に線分の端点がある場合の処理
	inline float GetMinDistSq_SegmentAABBFace_PointInEdgeArea(
		const  DirectX::XMVECTOR& targetPoint,
		const  DirectX::XMVECTOR& otherPoint,
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		const int u,
		const int v,
		DirectX::XMVECTOR* nearPointSegment,
		DirectX::XMVECTOR* nearPointAABB)
	{
		std::vector<DirectX::XMVECTOR> aabbFaceCorner;
		if (u == 1)
		{
			if (v == 2)
			{
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			}
			else if (v == 4)
			{
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			}
		}
		else if (u == 2)
		{
			if (v == 1)
			{
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			}
			else if (v == 4)
			{
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			}
		}
		else if (u == 3)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		}
		else if (u == 4)
		{
			if (v == 1)
			{
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			}
			else if (v == 2)
			{
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
				aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			}
		}
		else if (u == 5)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		}
		else if (u == 6)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		}
		else if (v == 3)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		}
		else if (v == 5)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		}
		else if (v == 6)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		}


		DirectX::XMVECTOR tmpNearPointSegment = {};
		DirectX::XMVECTOR tmpNearPointAABB = {};
		float minDist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[1], nearPointSegment, nearPointAABB);
		float dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[2], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[3], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[4], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[4], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}
		dist = GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[4], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
		if (dist < minDist)
		{
			minDist = dist;
			if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
			if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
		}

		return minDist;
	}


	// 線分とAABBの最短距離の二乗を取得する
	float GetMinDistSq_SegmentAABB(const DirectX::XMVECTOR& pointA,
		const DirectX::XMVECTOR& pointB,
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		DirectX::XMVECTOR* nearPointSegment = {},
		DirectX::XMVECTOR* nearPointAABB = {})
	{
		// 線分をレイと見立ててAABBと交差するか確認
		DirectX::XMVECTOR rayDirection = DirectX::XMVectorSubtract(pointB, pointA);
		float rayDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(rayDirection));
		rayDirection = DirectX::XMVector3Normalize(rayDirection);
		HitResultVector tmpResultNear = {}, tmpResultFar = {};
		float minDistanceSq = 0.0f;

		if (Collision3D::IntersectRayVsAABB(pointA, rayDirection, rayDist, aabbPos, aabbRadii, &tmpResultNear, &tmpResultFar))
		{
			if (nearPointSegment)
			{
				if (tmpResultNear.distance == 0.0f)
				{
					*nearPointSegment = pointA;
				}
				else
				{
					*nearPointSegment = pointB;
				}
			}
			if (nearPointAABB)
			{
				if (tmpResultNear.distance == 0.0f)
				{
					*nearPointAABB = tmpResultFar.position;
				}
				else
				{
					*nearPointAABB = tmpResultNear.position;
				}
			}

			return 0.0f;
		}
		else
		{
			int uA = 0, vA = 0;
			if (DirectX::XMVectorGetX(pointA) <= DirectX::XMVectorGetX(aabbPos) - DirectX::XMVectorGetX(aabbRadii))			uA |= (1 << 0);
			else if (DirectX::XMVectorGetX(pointA) >= DirectX::XMVectorGetX(aabbPos) + DirectX::XMVectorGetX(aabbRadii))	vA |= (1 << 0);
			if (DirectX::XMVectorGetY(pointA) <= DirectX::XMVectorGetY(aabbPos) - DirectX::XMVectorGetY(aabbRadii))			uA |= (1 << 1);
			else if (DirectX::XMVectorGetY(pointA) >= DirectX::XMVectorGetY(aabbPos) + DirectX::XMVectorGetY(aabbRadii))	vA |= (1 << 1);
			if (DirectX::XMVectorGetZ(pointA) <= DirectX::XMVectorGetZ(aabbPos) - DirectX::XMVectorGetZ(aabbRadii))			uA |= (1 << 2);
			else if (DirectX::XMVectorGetZ(pointA) >= DirectX::XMVectorGetZ(aabbPos) + DirectX::XMVectorGetZ(aabbRadii))	vA |= (1 << 2);
			int maskA = uA | vA;

			int uB = 0, vB = 0;
			if (DirectX::XMVectorGetX(pointB) <= DirectX::XMVectorGetX(aabbPos) - DirectX::XMVectorGetX(aabbRadii))			uB |= (1 << 0);
			else if (DirectX::XMVectorGetX(pointB) >= DirectX::XMVectorGetX(aabbPos) + DirectX::XMVectorGetX(aabbRadii))	vB |= (1 << 0);
			if (DirectX::XMVectorGetY(pointB) <= DirectX::XMVectorGetY(aabbPos) - DirectX::XMVectorGetY(aabbRadii))			uB |= (1 << 1);
			else if (DirectX::XMVectorGetY(pointB) >= DirectX::XMVectorGetY(aabbPos) + DirectX::XMVectorGetY(aabbRadii))	vB |= (1 << 1);
			if (DirectX::XMVectorGetZ(pointB) <= DirectX::XMVectorGetZ(aabbPos) - DirectX::XMVectorGetZ(aabbRadii))			uB |= (1 << 2);
			else if (DirectX::XMVectorGetZ(pointB) >= DirectX::XMVectorGetZ(aabbPos) + DirectX::XMVectorGetZ(aabbRadii))	vB |= (1 << 2);
			int maskB = uB | vB;

			// 同じ領域の場合
			if (uA == uB && vA == vB)
			{
				DirectX::XMVECTOR nearPointA = Collision3D::GetClosestPoint_PointAABB(pointA, aabbPos, aabbRadii);
				DirectX::XMVECTOR nearPointB = Collision3D::GetClosestPoint_PointAABB(pointB, aabbPos, aabbRadii);
				float nearPointDistA = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(pointA, nearPointA)));
				float nearPointDistB = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(pointB, nearPointB)));

				DirectX::XMVECTOR cornerA = GetAABBCorner(aabbPos, aabbRadii, vA);
				DirectX::XMVECTOR cornerB = GetAABBCorner(aabbPos, aabbRadii, 7 - uA);
				float edgeDist = GetMinDistSq_SegmentSegment(pointA, pointB, cornerA, cornerB, nearPointSegment, nearPointAABB);

				float ret = 0.0f;
				if (nearPointDistA < nearPointDistB)
				{
					if (edgeDist < nearPointDistA)
					{
						ret = edgeDist;
					}
					else
					{
						ret = nearPointDistA;
						if (nearPointSegment) *nearPointSegment = pointA;
						if (nearPointAABB) *nearPointAABB = nearPointA;
					}
				}
				else
				{
					if (edgeDist < nearPointDistB)
					{
						ret = edgeDist;
					}
					else
					{
						ret = nearPointDistB;
						if (nearPointSegment) *nearPointSegment = pointB;
						if (nearPointAABB) *nearPointAABB = nearPointB;
					}
				}

				return ret;
			}
			else
			{
				// それぞれ違う面領域の場合
				if ((maskA & (maskA - 1)) == 0 && (maskB & (maskB - 1)) == 0)
				{
					int uAdd = uA + uB;
					int vAdd = vA + vB;
					DirectX::XMVECTOR cornerA = GetAABBCorner(aabbPos, aabbRadii, vAdd);
					DirectX::XMVECTOR cornerB = GetAABBCorner(aabbPos, aabbRadii, 7 - uAdd);

					return GetMinDistSq_SegmentSegment(pointA, pointB, cornerA, cornerB, nearPointSegment, nearPointAABB);
				}
				// pointA側のみが面領域の場合
				else if ((maskA & (maskA - 1)) == 0)
				{
					return GetMinDistSq_SegmentAABBFace_PointInFaceArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
				}
				// pointB側のみが面領域の場合
				else if ((maskB & (maskB - 1)) == 0)
				{
					return GetMinDistSq_SegmentAABBFace_PointInFaceArea(pointB, pointA, aabbPos, aabbRadii, uB, vB, nearPointSegment, nearPointAABB);
				}
				// 両方が頂点領域の場合
				else if (maskA == 7 && maskB == 7)
				{
					return GetMinDistSq_SegmentAABBFace_PointInCornerArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
				}
				// pointA側のみが頂点領域の場合
				else if (maskA == 7)
				{
					return GetMinDistSq_SegmentAABBFace_PointInEdgeArea(pointB, pointA, aabbPos, aabbRadii, uB, vB, nearPointSegment, nearPointAABB);
				}
				// pointB側のみが頂点領域の場合
				else if (maskB == 7)
				{
					return GetMinDistSq_SegmentAABBFace_PointInEdgeArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
				}
				// 両方が辺領域の場合
				else
				{
					return GetMinDistSq_SegmentAABBFace_PointInEdgeArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
				}
			}
		}
	}

}

// 球Vs球
bool Collision3D::IntersectSphereVsSphere(
	const Vector3& s0Pos, float s0Radius,
	const Vector3& s1Pos, float s1Radius,
	Vector3& hitPosition, Vector3& hitNormal, float& penetration)
{
	return Collision3D::IntersectSphereVsSphere(
		DirectX::XMLoadFloat3(&s0Pos), s0Radius,
		DirectX::XMLoadFloat3(&s1Pos), s1Radius,
		hitPosition, hitNormal, penetration);
}

// 球Vs球
bool Collision3D::IntersectSphereVsSphere(
	const DirectX::XMVECTOR& s0Pos, float s0Radius,
	const DirectX::XMVECTOR& s1Pos, float s1Radius,
	Vector3& hitPosition, Vector3& hitNormal, float& penetration)
{
	// 距離チェック
	DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(s0Pos, s1Pos);
	float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(Direction));
	float sumRadi = (s0Radius + s1Radius);
	if (length > sumRadi)
		return false;

	DirectX::XMStoreFloat3(&hitPosition,
		DirectX::XMVectorScale(DirectX::XMVectorAdd(s0Pos, s1Pos), 0.5f));
	DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(Direction));
	penetration = sumRadi - length;
	return true;
}

// 球Vs三角形
bool Collision3D::IntersectSphereVsTriangle(const DirectX::XMVECTOR& spherePos, float RADIUS, const DirectX::XMVECTOR trianglePos[3])
{
	DirectX::XMVECTOR tmpPos = {};
	CollisionHelper::GetClosestPoint_PointTriangle(spherePos, trianglePos, tmpPos);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(spherePos, tmpPos);
	bool ret = false;
	DirectX::XMVECTOR vecNorm = DirectX::XMVector3Normalize(vec);
	ret = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec)) <= (RADIUS * RADIUS);
	return ret;
}

/// 球Vsカプセル
bool Collision3D::IntersectSphereVsCapsule(
	const Vector3& sPos, float sRadius,
	const Vector3& cPos, const Vector3& cDirection,
	float cRadius, float cLength,
	Vector3& hitPosition, Vector3& hitNormal, float& penetration)
{
	const DirectX::XMVECTOR SP = DirectX::XMLoadFloat3(&sPos);
	const DirectX::XMVECTOR CP = DirectX::XMLoadFloat3(&cPos);
	const DirectX::XMVECTOR CD = DirectX::XMLoadFloat3(&cDirection);
	const DirectX::XMVECTOR CD2SP = DirectX::XMVectorSubtract(SP, CD);
	const float sumRadi = cRadius + sRadius;

	// カプセルの始点から終点のベクトルに対して始点から球までのベクトルを射影
	float projection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CD2SP, CD));
	// 射影長を補正
	projection = std::clamp(projection, 0.0f, cLength);

	// 最近点算出
	const DirectX::XMVECTOR NearP = DirectX::XMVectorAdd(CP,
		DirectX::XMVectorScale(CD, projection));

	// あとは球Vs球
	return Collision3D::IntersectSphereVsSphere(
		SP, sRadius,
		NearP, cRadius,
		hitPosition, hitNormal, penetration);
}

/// 球Vsカプセル
bool Collision3D::IntersectSphereVsCapsule(
	const Vector3& sPos, float sRadius,
	const Vector3& cStart, const Vector3& cEnd,
	float cRadius,
	Vector3& hitPosition, Vector3& hitNormal, float& penetration)
{
	Vector3 direction = cEnd - cStart;
	float length = Vec3Length(direction);

	if (length != 0.0f)
	{
		direction /= length;
		return Collision3D::IntersectSphereVsCapsule(
			sPos, sRadius,
			cStart, direction, cRadius, length,
			hitPosition, hitNormal, penetration);
	}
	else
	{
		return Collision3D::IntersectSphereVsSphere(
			sPos, sRadius,
			cStart, cRadius,
			hitPosition, hitNormal, penetration);
	}
}

/// カプセルVsカプセル
bool Collision3D::IntersectCapsuleVsCapsule(
	const Vector3& c0Start, const Vector3& c0End,
	float c0Radius,
	const Vector3& c1Start, const Vector3& c1End,
	float c1Radius,
	Vector3& hitPosition, Vector3& hitNormal, float& penetration)
{
	const DirectX::XMVECTOR start0 = DirectX::XMLoadFloat3(&c0Start);
	const DirectX::XMVECTOR end0 = DirectX::XMLoadFloat3(&c0End);
	const DirectX::XMVECTOR start1 = DirectX::XMLoadFloat3(&c1Start);
	const DirectX::XMVECTOR end1 = DirectX::XMLoadFloat3(&c1End);

	DirectX::XMVECTOR point1 = {}, point2 = {};
	float distSq = CollisionHelper::GetMinDistSq_SegmentSegment(
		start0, end0,
		start1, end1,
		&point1, &point2);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);
	float radiusAdd = c0Radius + c1Radius;
	if (distSq < radiusAdd * radiusAdd)
	{
		DirectX::XMStoreFloat3(&hitPosition, DirectX::XMVectorScale(
			DirectX::XMVectorAdd(point1, point2), 0.5f));
		DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(vec));
		penetration = radiusAdd - sqrtf(distSq);

		return true;
	}

	return false;
}

// レイVs球
bool Collision3D::IntersectRayVsSphere(const DirectX::XMVECTOR& rayStart, const DirectX::XMVECTOR& rayDirection, float rayDist, const DirectX::XMVECTOR& spherePos, float RADIUS, HitResultVector* result)
{
	DirectX::XMVECTOR ray2sphere = DirectX::XMVectorSubtract(spherePos, rayStart);
	float projection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ray2sphere, rayDirection));
	float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ray2sphere)) - projection * projection;

	if (distSq < RADIUS * RADIUS)
	{
		float distance = projection - sqrtf(RADIUS * RADIUS - distSq);
		if (distance > 0.0f)
		{
			if (distance < rayDist)
			{
				if (result)
				{
					result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, distance));
					result->distance = distance;
					result->normal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(result->position, spherePos));
				}
				return true;
			}
		}
	}

	return false;
}

// レイVsAABB
bool Collision3D::IntersectRayVsAABB(const DirectX::XMVECTOR& rayStart, const DirectX::XMVECTOR& rayDirection, float rayDist, const DirectX::XMVECTOR& aabbPos, const DirectX::XMVECTOR& aabbRadii, HitResultVector* resultNear, HitResultVector* resultFar)
{
	HitResultVector nearPoint = {};
	HitResultVector farPoint = {};
	if (IntersectRayVsSlub3D(rayStart, rayDirection, rayDist, aabbPos, aabbRadii, &nearPoint, &farPoint))
	{
		if (nearPoint.distance < farPoint.distance)
		{
			if (resultNear)
			{
				*resultNear = nearPoint;
				if (resultFar)
				{
					*resultFar = farPoint;
				}
			}
			return true;
		}
	}

	return false;
}

// レイVsスラブ3D (軸平行スラブ)
bool Collision3D::IntersectRayVsSlub3D(const DirectX::XMVECTOR& rayStart, const DirectX::XMVECTOR& rayDirection, float rayDist, const DirectX::XMVECTOR& slubCenter, const DirectX::XMVECTOR& slubRadii, HitResultVector* resultNear, HitResultVector* resultFar)
{
	// ループ処理するため、xyz成分を配列に代入する
	float startPosArray[3] = { DirectX::XMVectorGetX(rayStart), DirectX::XMVectorGetY(rayStart), DirectX::XMVectorGetZ(rayStart) };
	float slubCenterArray[3] = { DirectX::XMVectorGetX(slubCenter), DirectX::XMVectorGetY(slubCenter), DirectX::XMVectorGetZ(slubCenter) };
	float slubRadiiArray[3] = { DirectX::XMVectorGetX(slubRadii), DirectX::XMVectorGetY(slubRadii), DirectX::XMVectorGetZ(slubRadii) };
	float dArray[3] = { DirectX::XMVectorGetX(rayDirection), DirectX::XMVectorGetY(rayDirection), DirectX::XMVectorGetZ(rayDirection) };

	// 直線とスラブの２交点までの距離をtminとtmaxと定義
	float tmin = 0.0f;
	float tmax = FLT_MAX;
	int minAxis = 0;

	bool ret = true;

	// スラブとの距離を算出し交差しているかの確認と最近点の算出を行う
	for (int i = 0; i < 3; i++)
	{
		//xyz軸との平行確認
		if (fabsf(dArray[i]) < FLT_EPSILON)
		{
			// 平行の場合、位置関係の比較を行い範囲内になければ交差なし
			if (startPosArray[i] < slubCenterArray[i] - slubRadiiArray[i] || startPosArray[i] > slubCenterArray[i] + slubRadiiArray[i])
			{
				ret = false;
			}
		}
		else
		{
			// t1が近スラブ、t2が遠スラブとの距離
			float ood = 1.0f / dArray[i];
			float t1 = (slubCenterArray[i] - slubRadiiArray[i] - startPosArray[i]) * ood;
			float t2 = (slubCenterArray[i] + slubRadiiArray[i] - startPosArray[i]) * ood;

			// 遠近が逆転している場合があるので、その場合入れ替えておく
			if (t1 > t2)
			{
				float tmp = t1;
				t1 = t2;
				t2 = tmp;
			}

			// t1がtminよりも大きい場合、tminをt1で更新する
			if (t1 > tmin)
			{
				tmin = t1;
				minAxis = i;
			}

			// t2がtmaxよりも小さい場合、tmaxをt2で更新する
			if (t2 < tmax)
			{
				tmax = t2;
			}
		}
	}

	if (resultNear)
	{
		resultNear->distance = tmin;
		resultNear->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, resultNear->distance));
		DirectX::XMVECTOR norm = { static_cast<float>(minAxis == 0),   static_cast<float>(minAxis == 1),  static_cast<float>(minAxis == 2) };
		if (dArray[minAxis] > 0)
		{
			norm = DirectX::XMVectorNegate(norm);
		}
		resultNear->normal = norm;

		if (resultFar)
		{
			resultFar->distance = tmax;
			resultFar->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, resultFar->distance));
			DirectX::XMVECTOR norm = { static_cast<float>(minAxis == 0),   static_cast<float>(minAxis == 1),  static_cast<float>(minAxis == 2) };
			if (dArray[minAxis] > 0)
			{
				norm = DirectX::XMVectorNegate(norm);
			}
			resultFar->normal = norm;
		}
	}

	return ret && (rayDist >= tmin);
}

// レイVs三角形
bool Collision3D::IntersectRayVsTriangle(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,
	float rayDist,
	const DirectX::XMVECTOR triangleVerts[3],
	HitResult& result)
{
	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(triangleVerts[1], triangleVerts[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(triangleVerts[2], triangleVerts[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(rayStart, DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, rayDist)));
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));
	if (d > 0.0f)	// 表側から交差しているときのみ判定を行う
	{
		if (fabs(d) > 1e-6f)	// 平行確認
		{
			DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStart, triangleVerts[0]);

			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
			if (t >= 0.0f && t < d)		// レイの向きと長さ確認
			{
				DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
				float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
				if (v >= 0.0f && v <= d)
				{
					float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
					if (w > 0.0f && v + w <= d)
					{
						result.distance = rayDist * t / d;
						DirectX::XMStoreFloat3(&result.position,
							DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, result.distance)));
						DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(norm));
						for (int i = 0; i < 3; i++)
						{
							DirectX::XMStoreFloat3(&result.triangleVerts[i], triangleVerts[i]);
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}

// 外部の点に対するAABB内部の最近点を取得する
DirectX::XMVECTOR Collision3D::GetClosestPoint_PointAABB(const DirectX::XMVECTOR& point, const DirectX::XMVECTOR& aabbPos, const DirectX::XMVECTOR& aabbRadii, bool surfaceFlg)
{
	float aabbPosArray[3] = { DirectX::XMVectorGetX(aabbPos), DirectX::XMVectorGetY(aabbPos), DirectX::XMVectorGetZ(aabbPos) };
	float aabbRadArray[3] = { DirectX::XMVectorGetX(aabbRadii), DirectX::XMVectorGetY(aabbRadii), DirectX::XMVectorGetZ(aabbRadii) };
	float nearPoint[3] = { DirectX::XMVectorGetX(point), DirectX::XMVectorGetY(point), DirectX::XMVectorGetZ(point) };

	int targetAxis = -1;
	int count = 0;
	float dist = FLT_MAX;

	for (int i = 0; i < 3; i++)
	{
		if (nearPoint[i] < aabbPosArray[i] - aabbRadArray[i])
		{
			nearPoint[i] = aabbPosArray[i] - aabbRadArray[i];
		}
		else if (nearPoint[i] > aabbPosArray[i] + aabbRadArray[i])
		{
			nearPoint[i] = aabbPosArray[i] + aabbRadArray[i];
		}
		else if (nearPoint[i] <= aabbPosArray[i])
		{
			if (fabsf(dist) > fabsf(aabbPosArray[i] - aabbRadArray[i] - nearPoint[i]))
			{
				targetAxis = i;
				dist = aabbPosArray[i] - aabbRadArray[i] - nearPoint[i];
			}
			count++;
		}
		else if (nearPoint[i] >= aabbPosArray[i])
		{
			if (fabsf(dist) > fabsf(aabbPosArray[i] + aabbRadArray[i] - nearPoint[i]))
			{
				targetAxis = i;
				dist = aabbPosArray[i] + aabbRadArray[i] - nearPoint[i];
			}
			count++;
		}
	}

	// surfaceFlgがtrueかつcountが3の場合、内部にあるため、最近点を表面に補正する
	if (surfaceFlg)
	{
		if (count == 3)
		{
			if (nearPoint[targetAxis] < aabbPosArray[targetAxis])
			{
				nearPoint[targetAxis] = aabbPosArray[targetAxis] - aabbRadArray[targetAxis];
			}
			else if (nearPoint[targetAxis] > aabbPosArray[targetAxis])
			{
				nearPoint[targetAxis] = aabbPosArray[targetAxis] + aabbRadArray[targetAxis];
			}
		}
	}

	return DirectX::XMVECTOR{ nearPoint[0], nearPoint[1], nearPoint[2] };
}

// レイVs円柱
bool Collision3D::IntersectRayVsOrientedCylinder(const DirectX::XMVECTOR& rayStart, const DirectX::XMVECTOR& rayDirection, float rayDist, const DirectX::XMVECTOR& startCylinder, const DirectX::XMVECTOR& endCylinder, float RADIUS, HitResultVector* result, DirectX::XMVECTOR* onCenterLinPos)
{
	const DirectX::XMVECTOR d = DirectX::XMVectorSubtract(endCylinder, startCylinder);
	const DirectX::XMVECTOR m = DirectX::XMVectorSubtract(rayStart, startCylinder);
	const DirectX::XMVECTOR n = DirectX::XMVectorScale(rayDirection, rayDist);

	const float md = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, d));
	const float nd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, d));
	const float dd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, d));

	// 線分全体が円柱の底面・上面に垂直なスラブに対して外側にあるかどうかを判定
	// レイの始点から終点がスラブPQに届いているか確認
	{
		// 底面の処理
		if (fabsf(md + nd) < 0)
		{
			return false;
		}
		// 上面の処理
		if (md > dd && md + nd > dd)
		{
			return false;
		}
	}


	const float nn = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, n));
	const float a = nn * dd - (nd * nd);
	const float mm = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, m));
	const float k = mm - RADIUS * RADIUS;
	//float c = mm * dd - (md * md) - (radius * radius) * dd;
	//float c = mm * dd - (radius * radius) * dd - (md * md);
	// ddで括る
	const float c = dd * k - (md * md);

	// 線分が円柱の軸に対して平行
	if (fabsf(a) < 0.0001f)		// 誤差が出やすい計算なので閾値は大きめ（0.0001f）
	{
		if (c > 0.0f) return false;	// 線分は円柱の外側

		if (result)
		{
			// 底面交差のresultを算出
			if (md < 0)
			{
				// レイの始点から底面交差点の距離を求める
				float length = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(d), m));
				// 距離がマイナスなら円柱の外部から内部に向かっているレイ
				if (length < 0.0f)
					length = -length;	// 距離をプラスに戻す
				// 距離を設定
				result->distance = length;
				// 最近点を算出
				result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, result->distance));
				// 法線は円柱から最近点の向き
				result->normal = DirectX::XMVectorSubtract(result->position, startCylinder);
				// startCylinder-endCylinder間の最近点
				if (onCenterLinPos)
					*onCenterLinPos = startCylinder;
			}
			// 上面交差のresultを算出
			else
			{
				DirectX::XMVECTOR QA = DirectX::XMVectorSubtract(rayStart, endCylinder);
				// レイの始点から上面交差点の距離を求める
				float length = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(d), QA));
				// 距離がマイナスなら円柱の外部から内部に向かっているレイ
				if (length < 0.0f)
					length = -length;	// 距離をプラスに戻す
				// 距離を設定
				result->distance = length;
				// 最近点を算出
				result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, result->distance));
				// 法線は円柱から最近点の向き
				result->normal = DirectX::XMVectorSubtract(result->position, endCylinder);
				// startCylinder-endCylinder間の最近点
				if (onCenterLinPos)
					*onCenterLinPos = endCylinder;
			}
			// 法線を正規化
			result->normal = DirectX::XMVector3Normalize(result->normal);
		}
		return true;
	}


	// 線分が円柱の軸に対して平行でない
	// 円柱の表面を表す陰関数方程式と直線の方程式の解を求めて交差判定を行う。
	const float mn = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, n));
	const float b = mn * dd - nd * md;
	const float D = b * b - a * c;	// 判別式

	if (D < 0) return false;	// 実数解がないので交差していない

	// 解の公式により、交点までの距離を算出
	float hitDistance = -b - sqrtf(D);
	if (hitDistance < 0.0f)
	{
		hitDistance = (-b + sqrtf(D));
		if (hitDistance < 0.0f)
		{
			return false;	// 交点が線分の外側にあり交差していない
		}
	}
	else if (hitDistance > a)
	{
		hitDistance = (-b + sqrtf(D));
		if (hitDistance > a)
		{
			return false;	// 交点が線分の外側にあり交差していない
		}
	}
	hitDistance /= a;

	// 解の公式の結果、円柱のstartCylinder側の底面の外で交差しているか確認
	if (md + hitDistance * nd < 0.0f)
	{
		// 底面との交差を確認し、交差していればhitDistanceを更新
		float dLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(d));
		DirectX::XMVECTOR AX = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, hitDistance));
		float tnDush = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(d), AX));
		float tDush = -md / nd;
		if (mm + 2.0f * tDush * mn + tDush * tDush * nn <= RADIUS * RADIUS)
		{
			if (tDush < 0.0f)// なぜか -md / nd がマイナスになる
				return false;
			//tDush *= -1.0f;
			hitDistance = tDush;
		}
		else
			return false;
	}
	// 解の公式の結果、円柱のendCylinder側の上面の外で交差しているか確認
	else if (md + hitDistance * nd > dd)
	{
		// 上面との交差を確認し、交差していればhitDistanceを更新
		float tDush = (dd - md) / nd;
		if (mm + 2.0f * tDush * mn - 2.0f * md + tDush * tDush * nn - 2.0f * tDush * nd + dd <= RADIUS * RADIUS)
		{
			if (tDush < 0.0f)// なぜか (dd - md) / nd がマイナスになる
				return false;
			//tDush *= -1.0f;
			hitDistance = tDush;
		}
		else
			return false;
	}

	// 線分が円柱の底面と上面の間で交差していることが確定（底面・上面での交差も含む）
	if (result)
	{
		// resultの各パラメータを算出する
		result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, hitDistance * rayDist));
		float nLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(n));
		result->distance = hitDistance * nLength;
		// 法線はstartCylinder-endCylinder間の最近点のから最近点の向き
		DirectX::XMVECTOR dNormal = DirectX::XMVector3Normalize(d);
		DirectX::XMVECTOR OB = DirectX::XMVectorSubtract(result->position, startCylinder);
		DirectX::XMVECTOR projection = DirectX::XMVector3Dot(dNormal, OB);
		DirectX::XMVECTOR OH = DirectX::XMVectorScale(dNormal, DirectX::XMVectorGetX(projection));
		result->normal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(OH, OB));


		if (onCenterLinPos)
		{
			*onCenterLinPos = DirectX::XMVectorAdd(OH, startCylinder);
		}
	}

	return true;
}

// スフィアキャストVs三角形
bool Collision3D::IntersectSphereCastVsTriangle(const DirectX::XMVECTOR& sphereCastStart,
	const DirectX::XMVECTOR& sphereCastDirection,
	float sphereCastDist,
	float sphereCastRadius,
	const DirectX::XMVECTOR trianglePos[3],
	HitResult* result,
	bool firstSphereChk)
{
	// 三角形とスタート位置における球が交差している場合は、falseで終了する
	if (firstSphereChk)
	{
		if (IntersectSphereVsTriangle(sphereCastStart, sphereCastRadius, trianglePos))
		{
			return false;
		}
	}

	// 三角形の法線を算出
	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR normalNorm = DirectX::XMVector3Normalize(norm);

	DirectX::XMVECTOR inverceDirection = DirectX::XMVectorScale(sphereCastDirection, -sphereCastDist);
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, inverceDirection));
	bool hitFlg = false;

	if (d >= 0.0f)	// 表側から交差しているときのみ判定を行う
	{
		// 三角形の各頂点を法線を元に球半径だけ移動させる
		DirectX::XMVECTOR fixVec = DirectX::XMVectorScale(normalNorm, sphereCastRadius);

		// 移動後の三角形とスフィアキャストの中心レイが交差するなら、元の三角形の内部(面領域)でスフィアキャストが交差することが確定
		DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(sphereCastStart, DirectX::XMVectorAdd(trianglePos[0], fixVec));
		float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));

		// 面領域判定
		if (t >= 0.0f && t < d)		// レイの向きと長さ確認
		{
			DirectX::XMVECTOR cross = DirectX::XMVector3Cross(inverceDirection, ap);
			float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, cross));
			if (v >= 0.0f && v <= d)
			{
				float w = -1.0f * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, cross));
				if (w > 0.0f && v + w <= d)
				{
					if (result)
					{
						DirectX::XMVECTOR crossPos = DirectX::XMVectorAdd(sphereCastStart, DirectX::XMVectorScale(inverceDirection, -t / d));
						result->distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(crossPos, sphereCastStart)));
						DirectX::XMStoreFloat3(&result->position, DirectX::XMVectorSubtract(crossPos, fixVec));
						DirectX::XMStoreFloat3(&result->normal, normalNorm);
						DirectX::XMStoreFloat3(&result->triangleVerts[0], trianglePos[0]);
						DirectX::XMStoreFloat3(&result->triangleVerts[1], trianglePos[1]);
						DirectX::XMStoreFloat3(&result->triangleVerts[2], trianglePos[2]);
					}
					return true;
				}
			}
		}

		// 面領域で交差がなければ、ボロノイの各頂点領域、辺領域で交差判定を行い、最短距離を算出する
		enum class IntersectPattern
		{
			enNone = -1,
			enVertex0 = 1,
			enVertex1,
			enVertex2,
			enEdge01,
			enEdge02,
			enEdge12
		};
		HitResultVector tmpResult = {};
		DirectX::XMVECTOR minPosition = {}, minNormal = {}, tmpOnCenterLinePos = {}, minOnCenterLinePos = {};
		float minDistance = sphereCastDist;
		IntersectPattern minDistCalcPattern = IntersectPattern::enNone;

		// trianglePos[0] 頂点領域のチェック
		if (IntersectRayVsSphere(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[0], sphereCastRadius, &tmpResult))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = trianglePos[0];
				minNormal = tmpResult.normal;
				minDistCalcPattern = IntersectPattern::enVertex0;
				hitFlg = true;
			}
		}

		// trianglePos[1] 頂点領域のチェック
		if (IntersectRayVsSphere(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[1], sphereCastRadius, &tmpResult))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = trianglePos[1];
				minNormal = tmpResult.normal;
				minDistCalcPattern = IntersectPattern::enVertex1;
				hitFlg = true;
			}
		}

		// trianglePos[2] 頂点領域のチェック
		if (IntersectRayVsSphere(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[2], sphereCastRadius, &tmpResult))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = trianglePos[2];
				minNormal = tmpResult.normal;
				minDistCalcPattern = IntersectPattern::enVertex2;
				hitFlg = true;
			}
		}

		// trianglePos[0]-trianglePos[1] 辺領域のチェック
		if (IntersectRayVsOrientedCylinder(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[0], trianglePos[1],
			sphereCastRadius, &tmpResult, &tmpOnCenterLinePos))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minOnCenterLinePos = tmpOnCenterLinePos;
				minDistCalcPattern = IntersectPattern::enEdge01;
				hitFlg = true;
			}
		}

		// trianglePos[0]-trianglePos[2] 辺領域のチェック
		if (IntersectRayVsOrientedCylinder(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[0], trianglePos[2],
			sphereCastRadius, &tmpResult, &tmpOnCenterLinePos))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minOnCenterLinePos = tmpOnCenterLinePos;
				minDistCalcPattern = IntersectPattern::enEdge02;
				hitFlg = true;
			}
		}

		// trianglePos[1]-trianglePos[2] 辺領域のチェック
		if (IntersectRayVsOrientedCylinder(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[1], trianglePos[2],
			sphereCastRadius, &tmpResult, &tmpOnCenterLinePos))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minOnCenterLinePos = tmpOnCenterLinePos;
				minDistCalcPattern = IntersectPattern::enEdge12;
				hitFlg = true;
			}
		}

		// 交差が確定し、resultが有効ならHitResult情報を算出する
		if (hitFlg && result)
		{
			// distance = レイの発射点である、球体の中心から、球体が三角形に当たった時の球体の中心までの距離
			result->distance = minDistance;
			DirectX::XMStoreFloat3(&result->triangleVerts[0], trianglePos[0]);
			DirectX::XMStoreFloat3(&result->triangleVerts[1], trianglePos[1]);
			DirectX::XMStoreFloat3(&result->triangleVerts[2], trianglePos[2]);
			// 三角形の法線
			DirectX::XMStoreFloat3(&result->normal, DirectX::XMVector3Normalize(minNormal));

			// 交点(result->position)の算出は当たり方によって分岐
			// position = 球体とモデルのメッシュとの最近点
			switch (minDistCalcPattern)
			{
			case IntersectPattern::enVertex0:
			case IntersectPattern::enVertex1:
			case IntersectPattern::enVertex2:
				DirectX::XMStoreFloat3(&result->position, minPosition);
				break;
			case IntersectPattern::enEdge01:
			case IntersectPattern::enEdge02:
			case IntersectPattern::enEdge12:
			{
				DirectX::XMStoreFloat3(&result->position, tmpOnCenterLinePos);
			}
			break;
			default:
				break;
			}
		}
	}

	return hitFlg;
}

// カプセルvsAABBの交差判定
bool Collision3D::IntersectCapsuleVsAABB(const DirectX::XMVECTOR& position, const DirectX::XMVECTOR& direction, const float length, const float RADIUS, const DirectX::XMVECTOR& aabbPos, const DirectX::XMVECTOR& aabbRadii)
{
	float dist = 0.0f;
	DirectX::XMVECTOR nearPointSegment = {};
	DirectX::XMVECTOR nearPointAABB = {};

	DirectX::XMVECTOR end = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, length));

	dist = CollisionHelper::GetMinDistSq_SegmentAABB(position, end, aabbPos, aabbRadii);

	if (dist < RADIUS * RADIUS)
	{
		return true;
	}

	return false;
}
