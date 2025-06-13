#pragma once

#include <string>
#include "../../Library/Math/Vector.h"

// 前方宣言
class Actor;

/// <summary>
/// 接触情報レイヤー
/// </summary>
enum class CollisionLayer : unsigned int
{
	None		= 1 << 0,
	Attack		= 1 << 1,
	Hit			= 1 << 2,

	CollisionLayerMAX,
};

/// <summary>
/// 接触情報
/// </summary>
struct CollisionData
{
	// 自分のレイヤー
	CollisionLayer myLayer = CollisionLayer::None;
	// 自分がトリガーかどうか
	bool isTrigger = false;
	// 衝突相手
	Actor* other = nullptr;
	// 衝突相手がトリガーかどうか
	bool otherIsTrigger = false;
	// 相手のレイヤー
	CollisionLayer otherLayer = CollisionLayer::None;
	// 衝突点
	Vector3 hitPosition = Vector3::Zero;
	// 衝突点の法線
	Vector3 hitNormal = Vector3::Zero;
	// めり込み量
	float penetration = 0.0f;
};

/// <summary>
/// 接触情報レイヤーマスク
/// ビットが立っているレイヤーと接触する
/// </summary>
typedef unsigned int CollisionLayerMask;
/// <summary>
/// すべてのレイヤーと接触するマスク
/// </summary>
static constexpr CollisionLayerMask CollisionLayerMaskAll = ~0;
/// <summary>
/// 指定のレイヤーと接触するマスク
/// </summary>
/// <param name="layer"></param>
/// <returns></returns>
static CollisionLayerMask GetCollisionLayerMask(CollisionLayer layer)
{
	return static_cast<CollisionLayerMask>(layer);
}
/// <summary>
/// 指定のレイヤー以外と接触するマスク
/// </summary>
/// <param name="layer"></param>
/// <returns></returns>
static CollisionLayerMask GetCollisionLayerMaskExcept(CollisionLayer layer)
{
	return ~static_cast<CollisionLayerMask>(layer);
}

/// <summary>
/// 互いに接触するか確認
/// </summary>
/// <param name="layerA"></param>
/// <param name="maskA"></param>
/// <param name="layerB"></param>
/// <param name="maskB"></param>
/// <returns>trueで接触可能</returns>
static bool CheckCollisionLayer(
	CollisionLayer layerA, CollisionLayerMask maskA,
	CollisionLayer layerB, CollisionLayerMask maskB)
{
	if ((static_cast<CollisionLayerMask>(layerA) & maskB) == 0 || 
		(static_cast<CollisionLayerMask>(layerB) & maskA) == 0)
	{
		// レイヤーがマスクに含まれていないなら衝突しない
		return false;
	}

	return true;
}
