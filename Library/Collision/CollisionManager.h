#pragma once

#include <vector>
#include <memory>

#include "../Graphics/RenderContext.h"

#include "../Component/Collider/SphereCollider.h"
#include "../Component/Collider/BoxCollider.h"
#include "../Component/Collider/CapsuleCollider.h"
#include "../Component/Collider/MeshCollider.h"

class CollisionManager
{
public:
	CollisionManager() {}
	~CollisionManager() {}

	// 更新処理
	void Update();

	// GUI描画処理
	void DrawGui();

#pragma region キャスト	
	/// <summary>
	/// レイキャスト
	/// </summary>
	/// <param name="start">レイ開始位置（ワールド）</param>
	/// <param name="direction">レイ方向（要正規化）</param>
	/// <param name="distance">IN OUT 距離</param>
	/// <param name="hitPosition">OUT 衝突点</param>
	/// <param name="hitNormal">OUT 衝突点の法線</param>
	/// <returns></returns>
	bool RayCast(
		const Vector3& start,
		const Vector3& direction,
		float& distance,
		Vector3& hitPosition,
		Vector3& hitNormal);

	/// <summary>
	/// スフィアキャスト
	/// </summary>
	/// <param name="origin">開始位置（ワールド）</param>
	/// <param name="direction">方向（要正規化）</param>
	/// <param name="radius">半径</param>
	/// <param name="distance">IN OUT 距離</param>
	/// <param name="hitPosition">OUT 衝突点</param>
	/// <param name="hitNormal">OUT 衝突点の法線</param>
	/// <returns></returns>
	bool SphereCast(
		const Vector3& origin,
		const Vector3& direction/*must normal*/,
		float radius,
		float& distance/*in out*/,
		Vector3& hitPosition/*out*/,
		Vector3& hitNormal/*out*/);
#pragma endregion

public:
#pragma region 登録
	// 球コライダー登録
	void RegisterSphereCollider(SphereCollider* sphereCollider);
	// ボックスコライダー登録	
	void RegisterBoxCollider(BoxCollider* boxCollider);
	// カプセルコライダー登録
	void RegisterCapsuleCollider(CapsuleCollider* capsuleCollider);
	// メッシュコライダー登録
	void RegisterMeshCollider(MeshCollider* meshCollider);
#pragma endregion

#pragma region 削除
	// 球コライダー削除
	void UnregisterSphereCollider(SphereCollider* sphereCollider);
	// ボックスコライダー削除
	void UnregisterBoxCollider(BoxCollider* boxCollider);
	// カプセルコライダー削除
	void UnregisterCapsuleCollider(CapsuleCollider* capsuleCollider);
	// メッシュコライダー削除
	void UnregisterMeshCollider(MeshCollider* meshCollider);
#pragma endregion

private:
	std::vector<SphereCollider*>	_sphereColliders; // 球コライダー
	std::vector<BoxCollider*>		_boxColliders; // ボックスコライダー
	std::vector<CapsuleCollider*>	_capsuleColliders; // カプセルコライダー
	std::vector<MeshCollider*>		_meshColliders; // メッシュコライダー
};