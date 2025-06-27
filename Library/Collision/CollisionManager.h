#pragma once

#include <vector>
#include <memory>
#include <mutex>

#include "CollisionDefine.h"
#include "../Graphics/RenderContext.h"

#include "../Component/Collider/SphereCollider.h"
#include "../Component/Collider/BoxCollider.h"
#include "../Component/Collider/CapsuleCollider.h"
#include "../Component/Collider/MeshCollider.h"

/// <summary>
/// Colliderの管理者
/// </summary>
class CollisionManager
{
public:
    struct SphereData
    {
		SphereData(Actor* actor, CollisionLayer layer, CollisionLayerMask mask, const Vector3& position, float radius, bool isTrigger)
		{
            this->actor = actor;
            this->layer = layer;
			this->mask = mask;
            this->position = position;
            this->radius = radius;
            this->isTrigger = isTrigger;
		}

        Actor* actor = nullptr;
		CollisionLayer layer = CollisionLayer::None;
		CollisionLayerMask mask = 0;
		Vector3 position = {};
        float radius = 0.0f;
        bool isTrigger = false;
    };
    struct BoxData
    {
        BoxData(Actor* actor, CollisionLayer layer, CollisionLayerMask mask, const Vector3& position, const Vector3& halfSize, const Vector3& rotation, bool isTrigger)
        {
            this->actor = actor;
            this->layer = layer;
			this->mask = mask;
            this->position = position;
            this->halfSize = halfSize;
            this->rotation = rotation;
			this->isTrigger = isTrigger;
        }

        Actor* actor;
		CollisionLayer layer = CollisionLayer::None;
		CollisionLayerMask mask = 0;
		Vector3 position;
		Vector3 halfSize;
        Vector3 rotation;
		bool isTrigger = false;
    };
    struct CapsuleData
    {
        CapsuleData(Actor* actor, CollisionLayer layer, CollisionLayerMask mask, const Vector3& start, const Vector3& end, float radius, bool isTrigger)
        {
            this->actor = actor;
            this->layer = layer;
			this->mask = mask;
            this->start = start;
            this->end = end;
            this->radius = radius;
			this->isTrigger = isTrigger;
        }

        Actor* actor;
		CollisionLayer layer = CollisionLayer::None;
		CollisionLayerMask mask = 0;
        Vector3 start;
        Vector3 end;
        float radius;
		bool isTrigger = false;
    };

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

    // 球データ登録
    void RegisterSphereData(Actor* actor, CollisionLayer layer, CollisionLayerMask mask, const Vector3& position, float radius, bool isTrigger = true);
    // ボックスデータ登録
    void RegisterBoxData(Actor* actor, CollisionLayer layer, CollisionLayerMask mask, const Vector3& position, const Vector3& halfSize, const Vector3& rotation, bool isTrigger = true);
    // カプセルデータ登録
    void RegisterCapsuleData(Actor* actor, CollisionLayer layer, CollisionLayerMask mask, const Vector3& start, const Vector3& end, float radius, bool isTrigger = true);
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
	/// <summary>
	/// 各コンポーネントの情報を設定
	/// </summary>
	void SetDataByCollider();

	// コンテナに情報を追加
	void PushCollisionData(
		std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap,
		Actor* actorA,
		CollisionLayer layerA,
		bool isTriggerA,
		Actor* actorB,
		CollisionLayer layerB,
		bool isTriggerB,
		const Vector3& hitPosition,
		const Vector3& hitNormal,
		float penetration);

#pragma region 各当たり判定
	void SphereVsSphere(
		const SphereData& sphereA,
		const SphereData& sphereB,
		std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap);
	void SphereVsBox(
		const SphereData& sphere,
		const BoxData& box,
		std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap);
	void SphereVsCapsule(
		const SphereData& sphere,
		const CapsuleData& capsule,
		std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap);
	void SphereVsMesh(
		const SphereData& sphere,
		std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap);
	void BoxVsBox(
		const BoxData& boxA,
		const BoxData& boxB,
		std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap);
	void CapsuleVsCapsule(
		const CapsuleData& capsuleA,
		const CapsuleData& capsuleB,
		std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap);
#pragma endregion


private:
	std::vector<SphereCollider*>	_sphereColliders; // 球コライダー
	std::vector<BoxCollider*>		_boxColliders; // ボックスコライダー
	std::vector<CapsuleCollider*>	_capsuleColliders; // カプセルコライダー
	std::vector<MeshCollider*>		_meshColliders; // メッシュコライダー

	// 衝突データ
    std::vector<SphereData> _sphereDatas;
    std::vector<BoxData> _boxDatas;
    std::vector<CapsuleData> _capsuleDatas;

    std::mutex _mutex; // スレッドセーフ用
};