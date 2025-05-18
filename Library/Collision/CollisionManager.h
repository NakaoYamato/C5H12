#pragma once

#include <vector>
#include <memory>
#include <mutex>

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
		SphereData(Actor* actor, std::string layer, const Vector3& position, float radius)
		{
            this->actor = actor;
            this->layer = layer;
            this->position = position;
            this->radius = radius;
		}

        Actor* actor = nullptr;
		std::string layer = "";
		Vector3 position = {};
        float radius = 0.0f;
    };
    struct BoxData
    {
        BoxData(Actor* actor, std::string layer, const Vector3& position, const Vector3& halfSize, const Vector3& rotation)
        {
            this->actor = actor;
            this->layer = layer;
            this->position = position;
            this->halfSize = halfSize;
            this->rotation = rotation;
        }

        Actor* actor;
		std::string layer = "";
		Vector3 position;
		Vector3 halfSize;
        Vector3 rotation;
    };
    struct CapsuleData
    {
        CapsuleData(Actor* actor, std::string layer, const Vector3& start, const Vector3& end, float radius)
        {
            this->actor = actor;
            this->layer = layer;
            this->start = start;
            this->end = end;
            this->radius = radius;
        }

        Actor* actor;
		std::string layer = "";
        Vector3 start;
        Vector3 end;
        float radius;
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
    void RegisterSphereData(Actor* actor, std::string layer, const Vector3& position, float radius);
    // ボックスデータ登録
    void RegisterBoxData(Actor* actor, std::string layer, const Vector3& position, const Vector3& halfSize, const Vector3& rotation);
    // カプセルデータ登録
    void RegisterCapsuleData(Actor* actor, std::string layer, const Vector3& start, const Vector3& end, float radius);
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