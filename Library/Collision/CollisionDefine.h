#pragma once

#include <string>
#include "../../Library/Math/Vector.h"

// 前方宣言
class Actor;

/// <summary>
/// 接触情報レイヤー
/// </summary>
enum class CollisionLayer
{
	None,
	Attack,
	Hit,
	Body,

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
