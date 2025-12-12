#pragma once

#include "../Math/Vector.h"
#include "../Model/Model.h"

// ヒット結果
struct HitResult
{
    // レイとポリゴンの交点
    Vector3 position = Vector3::Zero;
    // 衝突したポリゴンの法線ベクトル
    Vector3 normal = Vector3::Up;
    // レイの始点から交点までの距離
    float distance = 0.0f;
    // 衝突したポリゴンのマテリアル番号
    int materialIndex = -1;
    // 回転量
    Vector3 rotation = Vector3::Zero;
	// 接触した三角形の頂点情報
    Vector3	triangleVerts[3]{};
    int flag = false;
};
struct HitResultVector
{
    // レイとポリゴンの交点
    DirectX::XMVECTOR	position = { 0, 0, 0 };
    // 衝突したポリゴンの法線ベクトル
    DirectX::XMVECTOR	normal = { 0, 0, 0 };
    // レイの始点から交点までの距離
    float				distance = 0.0f;
    // 接触した三角形の頂点情報
    DirectX::XMVECTOR	triangleVerts[3] = {};
    // 衝突したポリゴンのマテリアル番号
    int					materialIndex = -1;
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
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

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
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

    /// <summary>
    /// 球VsAABB
    /// </summary>
    /// <param name="spherePos">球の位置</param>
    /// <param name="sphereRadius">球の半径</param>
    /// <param name="aabbCenter">AABBの中心座標</param>
    /// <param name="aabbRadii">AABBの半辺長</param>
    /// <returns></returns>
    bool IntersectSphereVsAABB(
        const Vector3& spherePos,
        float sphereRadius,
        const Vector3& aabbCenter,
        const Vector3& aabbRadii);

    /// <summary>
    /// 球VsAABB
    /// </summary>
    /// <param name="spherePos">球の位置</param>
    /// <param name="sphereRadius">球の半径</param>
    /// <param name="aabbCenter">AABBの中心座標</param>
    /// <param name="aabbRadii">AABBの半辺長</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectSphereVsAABB(
        const Vector3& spherePos,
        float sphereRadius,
        const Vector3& aabbCenter,
        const Vector3& aabbRadii,
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

    /// <summary>
    /// 球Vsボックス
    /// </summary>
    /// <param name="spherePos">球の位置</param>
    /// <param name="sphereRadius">球の半径</param>
    /// <param name="boxPos">ボックスの中心座標</param>
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
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

    /// <summary>
    /// 球Vsボックス
    /// </summary>
    /// <param name="spherePos">球の位置</param>
    /// <param name="sphereRadius">球の半径</param>
    /// <param name="boxMatrix">ボックスのトランスフォーム行列</param>
    /// <param name="boxLocalRadii">ボックスのローカル半辺長</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectSphereVsBox(
        const Vector3& spherePos,
        float sphereRadius,
        const DirectX::XMFLOAT4X4& boxMatrix,
        const Vector3& boxLocalRadii,
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

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
    /// 球Vs三角形
    /// </summary>
    /// <param name="spherePos">球の位置</param>
    /// <param name="radius">球の半径</param>
    /// <param name="trianglePos">三角形の各頂点</param>
    /// <returns></returns>
    bool IntersectSphereVsTriangle(
        const DirectX::XMVECTOR& spherePos,
        float radius,
        const DirectX::XMVECTOR trianglePos[3],
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

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
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

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
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

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
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

    /// <summary>
    /// AABBVsカプセル
    /// </summary>
    /// <param name="aabbPos">AABBの中心座標</param>
    /// <param name="aabbRadii">AABBの半辺長</param>
    /// <param name="cStart">カプセルの始点</param>
    /// <param name="cEnd">カプセルの終点</param>
    /// <param name="cRadius">カプセルの半径</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectAABBVsCapsule(
        const Vector3& aabbPos,
        const Vector3& aabbRadii,
        const Vector3& cStart,
        const Vector3& cEnd,
        float cRadius,
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

    /// <summary>
    /// ボックスVsカプセル
    /// </summary>
    /// <param name="boxPos">ボックスの位置</param>
    /// <param name="boxRadii">ボックスの半辺長</param>
    /// <param name="boxAngle">ボックスの回転量</param>
    /// <param name="cStart">カプセルの始点</param>
    /// <param name="cEnd">カプセルの終点</param>
    /// <param name="cRadius">カプセルの半径</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectBoxVsCapsule(
        const Vector3& boxPos,
        const Vector3& boxRadii,
        const Vector3& boxAngle,
        const Vector3& cStart,
        const Vector3& cEnd,
        float cRadius,
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

	/// <summary>
	/// ボックスVs点
	/// </summary>
    /// <param name="boxPos">ボックスの位置</param>
    /// <param name="boxRadii">ボックスの半辺長</param>
    /// <param name="boxAngle">ボックスの回転量</param>
	/// <param name="pointPos">点</param>
	/// <returns></returns>
	bool IntersectBoxVsPoint(
        const Vector3& boxPos,
        const Vector3& boxRadii,
        const Vector3& boxAngle,
		const Vector3& pointPos);

    /// <summary>
	/// ボックスVs点
    /// </summary>
    /// <param name="boxMatrix">ボックスのトランスフォーム行列</param>
    /// <param name="boxLocalRadii">ボックスのローカル半辺長</param>
    /// <param name="pointPos">点</param>
    /// <returns></returns>
    bool IntersectBoxVsPoint(
        const DirectX::XMFLOAT4X4& boxMatrix,
        const Vector3& boxLocalRadii,
        const Vector3& pointPos);

    /// <summary>
    /// カプセルVsカプセル
    /// </summary>
    /// <param name="c0Start">カプセル0始点</param>
    /// <param name="c0End">カプセル0終点</param>
    /// <param name="c0Radius">カプセル0半径</param>
    /// <param name="c1Start">カプセル1始点</param>
    /// <param name="c1End">カプセル1終点</param>
    /// <param name="c1Radius">カプセル1半径</param>
    /// <param name="hitPosition">衝突位置</param>
    /// <param name="hitNormal">衝突位置の法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    bool IntersectCapsuleVsCapsule(
        const Vector3& c0Start,
        const Vector3& c0End,
        float c0Radius,
        const Vector3& c1Start,
        const Vector3& c1End,
        float c1Radius,
        Vector3* hitPosition,
        Vector3* hitNormal,
        float* penetration);

    /// <summary>
    /// レイVs球
    /// </summary>
    /// <param name="rayStart">レイ始点</param>
    /// <param name="rayDirection">レイ方向（要正規化）</param>
    /// <param name="rayDist">レイ距離</param>
    /// <param name="spherePos">球位置</param>
    /// <param name="radius">球半径</param>
    /// <param name="result">結果</param>
    /// <returns></returns>
    bool IntersectRayVsSphere(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,
        float rayDist,
        const DirectX::XMVECTOR& spherePos,
        float radius,
        HitResultVector* result = nullptr);
    /// <summary>
    /// レイVs球
    /// </summary>
    /// <param name="rayStart">レイ始点</param>
    /// <param name="rayDirection">レイ方向（要正規化）</param>
    /// <param name="rayDist">レイ距離</param>
    /// <param name="spherePos">球位置</param>
    /// <param name="radius">球半径</param>
    /// <param name="result">結果</param>
    /// <returns></returns>
    bool IntersectRayVsSphere(
        const Vector3& rayStart,
        const Vector3& rayDirection,
        float rayDist,
        const Vector3& spherePos,
        float radius,
        HitResultVector* result = nullptr);

    /// <summary>
    /// レイVsAABB
    /// </summary>
    /// <param name="rayStart">レイ始点</param>
    /// <param name="rayDirection">レイ方向（要正規化）</param>
    /// <param name="rayDist">レイ距離</param>
    /// <param name="aabbPos">AABB中心座標</param>
    /// <param name="aabbRadii">AABB半辺長</param>
    /// <param name="resultNear">最初の交点情報</param>
    /// <param name="resultFar">最後の交点情報</param>
    /// <returns></returns>
    inline bool IntersectRayVsAABB(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,
        float rayDist,
        const DirectX::XMVECTOR& aabbPos,
        const DirectX::XMVECTOR& aabbRadii,
        HitResultVector* resultNear,
        HitResultVector* resultFar);

    /// <summary>
    /// レイVsスラブ3D (軸平行スラブ)
    /// </summary>
    /// <param name="rayStart">レイ始点</param>
    /// <param name="rayDirection">レイ方向（要正規化）</param>
    /// <param name="rayDist">レイ距離</param>
    /// <param name="slubCenter">スラブ中心座標</param>
    /// <param name="slubRadii">スラブ半辺長</param>
    /// <param name="resultNear">最初の交点情報</param>
    /// <param name="resultFar">最後の交点情報</param>
    /// <returns></returns>
    bool IntersectRayVsSlub3D(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,
        float rayDist,
        const DirectX::XMVECTOR& slubCenter,
        const DirectX::XMVECTOR& slubRadii,
        HitResultVector* resultNear = nullptr,
        HitResultVector* resultFar = nullptr);

    /// <summary>
    /// レイVs三角形
    /// </summary>
    /// <param name="rayStart">レイ始点</param>
    /// <param name="rayDirection">レイ方向（要正規化）</param>
    /// <param name="rayDist">レイ距離</param>
    /// <param name="triangleVerts">3角形の頂点</param>
    /// <param name="result">結果</param>
    /// <returns></returns>
    bool IntersectRayVsTriangle(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,
        float rayDist,
        const DirectX::XMVECTOR triangleVerts[3],
        HitResult& result);

    /// <summary>
    /// 外部の点に対するAABB内部の最近点を取得する
    /// </summary>
    /// <param name="point">外部の点</param>
    /// <param name="aabbPos">AABB中心座標</param>
    /// <param name="aabbRadii">AABB半辺長</param>
    /// <param name="surfaceFlg"></param>
    /// <returns></returns>
    DirectX::XMVECTOR GetClosestPoint_PointAABB(
        const DirectX::XMVECTOR& point,
        const DirectX::XMVECTOR& aabbPos,
        const DirectX::XMVECTOR& aabbRadii,
        bool surfaceFlg = false);

    /// <summary>
    /// レイVs円柱
    /// </summary>
    /// <param name="rayStart">レイ始点</param>
    /// <param name="rayDirection">レイ方向（要正規化）</param>
    /// <param name="rayDist">レイ距離</param>
    /// <param name="startCylinder">円柱始点</param>
    /// <param name="endCylinder">円柱終点</param>
    /// <param name="radius">円柱半径</param>
    /// <param name="result">結果</param>
    /// <param name="onCenterLinPos"></param>
    /// <returns></returns>
    bool IntersectRayVsOrientedCylinder(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// 要正規化
        float rayDist,
        const DirectX::XMVECTOR& startCylinder,
        const DirectX::XMVECTOR& endCylinder,
        float radius,
        HitResultVector* result = nullptr,
        DirectX::XMVECTOR* onCenterLinPos = nullptr);

    // スフィアキャストVs三角形

    /// <summary>
    /// スフィアキャストVs三角形
    /// </summary>
    /// <param name="sphereCastStart">スフィアキャスト始点</param>
    /// <param name="sphereCastDirection">スフィアキャスト方向（要正規化）</param>
    /// <param name="sphereCastDist">スフィアキャスト距離</param>
    /// <param name="sphereCastRadius">スフィアキャスト半径</param>
    /// <param name="trianglePos">3角形の頂点</param>
    /// <param name="result">結果</param>
    /// <param name="firstSphereChk"> スフィアキャストの発射点の球が既に三角形に接している場合にfalseを返すチェックを行うかどうか</param>
    /// <returns></returns>
    bool IntersectSphereCastVsTriangle(
        const DirectX::XMVECTOR& sphereCastStart,
        const DirectX::XMVECTOR& sphereCastDirection,
        float sphereCastDist,
        float sphereCastRadius,
        const DirectX::XMVECTOR trianglePos[3],
        HitResult* result = nullptr,
        bool firstSphereChk = false);

    /// <summary>
    /// カプセルvsAABBの交差判定
    /// </summary>
    /// <param name="position">カプセル始点</param>
    /// <param name="direction">カプセル始点から終点までの向き（要正規化）</param>
    /// <param name="length">カプセル長さ</param>
    /// <param name="radius">カプセル半径</param>
    /// <param name="aabbPos">AABB中心座標</param>
    /// <param name="aabbRadii">AABB半辺長</param>
    /// <returns></returns>
    bool IntersectCapsuleVsAABB(
        const DirectX::XMVECTOR& position,
        const DirectX::XMVECTOR& direction,
        const float				length,
        const float				radius,
        const DirectX::XMVECTOR& aabbPos,
        const DirectX::XMVECTOR& aabbRadii);
}

namespace Collision2D
{
    /// <summary>
    /// 点vsABB
    /// </summary>
    /// <param name="point">点座標</param>
    /// <param name="aabbPos">AABB中心座標</param>
    /// <param name="aabbSize">AABB半辺長</param>
    /// <returns></returns>
    bool IntersectPointVsAABB(
        const Vector2& point,
        const Vector2& aabbPos,
        const Vector2& aabbSize
    );
}