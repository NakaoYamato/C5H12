#pragma once

#include "../Math/Vector.h"
#include "../3D/Model.h"

// ヒット結果
struct HitResult
{
    Vector3 position = { 0,0,0 };// レイとポリゴンの交点
    Vector3 normal = { 0,0,0 };// 衝突したポリゴンの法線ベクトル
    float distance = 0.0f;// レイの始点から交点までの距離
    int materialIndex = -1;// 衝突したポリゴンのマテリアル番号
    Vector3 rotation = { 0,0,0 };// 回転量
    Vector3	triangleVerts[3];
    int flag = false;
};
struct HitResultVector
{
    DirectX::XMVECTOR	position = { 0, 0, 0 };// レイとポリゴンの交点
    DirectX::XMVECTOR	normal = { 0, 0, 0 };	// 衝突したポリゴンの法線ベクトル
    float				distance = 0.0f; 		// レイの始点から交点までの距離
    DirectX::XMVECTOR	triangleVerts[3] = {};
    int					materialIndex = -1; 	// 衝突したポリゴンのマテリアル番号
};

namespace Collision3D
{
    /// <summary>
    /// 球Vs球
    /// </summary>
    /// <param name="s0Pos">球0位置</param>
    /// <param name="s0Radius">球0半径</param>
    /// <param name="s1Pos">球1位置</param>
    /// <param name="s1Radius">球1半径</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectSphereVsSphere(
        const Vector3& s0Pos,
        float s0Radius,
        const Vector3& s1Pos,
        float s1Radius,
        Vector3& hitPosition,
        Vector3& hitNormal,
        float& penetration);

    /// <summary>
    /// 球Vs球
    /// </summary>
    /// <param name="s0Pos">球0位置</param>
    /// <param name="s0Radius">球0半径</param>
    /// <param name="s1Pos">球1位置</param>
    /// <param name="s1Radius">球1半径</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectSphereVsSphere(
        const DirectX::XMVECTOR& s0Pos,
        float s0Radius,
        const DirectX::XMVECTOR& s1Pos,
        float s1Radius,
        Vector3& hitPosition,
        Vector3& hitNormal,
        float& penetration);

    /// <summary>
    /// 球Vsボックス
    /// </summary>
    /// <param name="spherePos">球の位置</param>
    /// <param name="sphereRadius">球の半径</param>
    /// <param name="boxPos">ボックスの位置</param>
    /// <param name="boxRadii">ボックスの半辺長</param>
    /// <param name="boxAngle">ボックスの回転量</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectSphereVsBox(
        const Vector3& spherePos,
        float sphereRadius,
        const Vector3& boxPos,
        const Vector3& boxRadii,
        const Vector3& boxAngle,
        Vector3& hitPosition,
        Vector3& hitNormal,
        float& penetration);

    /// <summary>
    /// 球Vs三角形
    /// </summary>
    /// <param name="spherePos">球の位置</param>
    /// <param name="radius">球の半径</param>
    /// <param name="trianglePos">三角形の各頂点</param>
    /// <returns></returns>
    bool IntersectSphereVsTriangle(
        const DirectX::XMVECTOR& spherePos,
        float radius,
        const DirectX::XMVECTOR trianglePos[3]);

    /// <summary>
    /// 球Vsカプセル
    /// </summary>
    /// <param name="sPos">球の位置</param>
    /// <param name="sRadius">球の半径</param>
    /// <param name="cPos">カプセルの始点</param>
    /// <param name="cDirection">カプセルの始点から終点までのベクトル（要正規化）</param>
    /// <param name="cRadius">カプセルの半径</param>
    /// <param name="cLength">カプセルの長さ</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectSphereVsCapsule(
        const Vector3& sPos,
        float sRadius,
        const Vector3& cPos,
        const Vector3& cDirection,
        float cRadius,
        float cLength,
        Vector3& hitPosition,
        Vector3& hitNormal,
        float& penetration);

    /// <summary>
    /// 球Vsカプセル
    /// </summary>
    /// <param name="sPos">球の位置</param>
    /// <param name="sRadius">球の半径</param>
    /// <param name="cStart">カプセルの始点</param>
    /// <param name="cEnd">カプセルの終点</param>
    /// <param name="cRadius">カプセルの半径</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectSphereVsCapsule(
        const Vector3& sPos,
        float sRadius,
        const Vector3& cStart,
        const Vector3& cEnd,
        float cRadius,
        Vector3& hitPosition,
        Vector3& hitNormal,
        float& penetration);

    /// <summary>
    /// ボックスVsボックス
    /// </summary>
    /// <param name="box0Pos">ボックス0の位置</param>
    /// <param name="box0Radii">ボックス0の半辺長</param>
    /// <param name="box0Angle">ボックス0の回転量</param>
    /// <param name="box1Pos">ボックス1の位置</param>
    /// <param name="box1Radii">ボックス1の半辺長</param>
    /// <param name="box1Angle">ボックス1の回転量</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectBoxVsBox(
        const Vector3& box0Pos,
        const Vector3& box0Radii,
        const Vector3& box0Angle,
        const Vector3& box1Pos,
        const Vector3& box1Radii,
        const Vector3& box1Angle,
        Vector3& hitPosition,
        Vector3& hitNormal,
        float& penetration);

    /// <summary>
    /// カプセルVsカプセル
    /// </summary>
    /// <param name="c0Start"></param>
    /// <param name="c0End"></param>
    /// <param name="c0Radius"></param>
    /// <param name="c1Start"></param>
    /// <param name="c1End"></param>
    /// <param name="c1Radius"></param>
    /// <param name="hitPosition"></param>
    /// <param name="hitNormal"></param>
    /// <param name="penetration"></param>
    /// <returns></returns>
    bool IntersectCapsuleVsCapsule(
        const Vector3& c0Start,
        const Vector3& c0End,
        float c0Radius,
        const Vector3& c1Start,
        const Vector3& c1End,
        float c1Radius,
        Vector3& hitPosition,
        Vector3& hitNormal,
        float& penetration);

    // レイVs球
    bool IntersectRayVsSphere(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// 要正規化
        float rayDist,
        const DirectX::XMVECTOR& spherePos,
        float RADIUS,
        HitResultVector* result = {});

    // レイVsAABB
    inline bool IntersectRayVsAABB(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// 要正規化
        float rayDist,
        const DirectX::XMVECTOR& aabbPos,
        const DirectX::XMVECTOR& aabbRadii,
        HitResultVector* resultNear,
        HitResultVector* resultFar);

    // レイVsスラブ3D (軸平行スラブ)
    bool IntersectRayVsSlub3D(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// 要正規化
        float rayDist,
        const DirectX::XMVECTOR& slubCenter,
        const DirectX::XMVECTOR& slubRadii,
        HitResultVector* resultNear = {},			// レイとスラブの最初の交点情報
        HitResultVector* resultFar = {});			// レイとスラブの最後の交点情報

    // レイVs三角形
    bool IntersectRayVsTriangle(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// 要正規化
        float rayDist,
        const DirectX::XMVECTOR triangleVerts[3],
        HitResult& result);

    // 外部の点に対するAABB内部の最近点を取得する
    DirectX::XMVECTOR GetClosestPoint_PointAABB(
        const DirectX::XMVECTOR& point,
        const DirectX::XMVECTOR& aabbPos,
        const DirectX::XMVECTOR& aabbRadii,
        bool surfaceFlg = false);

    // レイVs円柱
    bool IntersectRayVsOrientedCylinder(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// 要正規化
        float rayDist,
        const DirectX::XMVECTOR& startCylinder,
        const DirectX::XMVECTOR& endCylinder,
        float RADIUS,
        HitResultVector* result = {},
        DirectX::XMVECTOR* onCenterLinPos = {});

    // スフィアキャストVs三角形
    bool IntersectSphereCastVsTriangle(
        const DirectX::XMVECTOR& sphereCastStart,		// スフィアキャストの発射点（球の中心）
        const DirectX::XMVECTOR& sphereCastDirection,	// スフィアキャストの向き。要正規化
        float sphereCastDist,
        float sphereCastRadius,							// スフィアキャストの半径
        const DirectX::XMVECTOR trianglePos[3],
        HitResult* result = {},
        bool firstSphereChk = false);				// スフィアキャストの発射点の球が既に三角形に接している場合にfalseを返すチェックを行うかどうか

    // カプセルvsAABBの交差判定				※カプセル中心間のレイがAABBと交差している場合の押し戻しのためのresultはレイ方向に動く仕様。レイ以外の最短方向ではない。
    bool IntersectCapsuleVsAABB(
        const DirectX::XMVECTOR& position,	// 中心
        const DirectX::XMVECTOR& direction,	// 向き（正規化）
        const float				length,	// 長さ
        const float				RADIUS,	// 半径
        const DirectX::XMVECTOR& aabbPos,
        const DirectX::XMVECTOR& aabbRadii);
}