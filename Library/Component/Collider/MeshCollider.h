#pragma once

#include <DirectXCollision.h>
#include <mutex>
#include "ColliderBase.h"

class MeshCollider : public ColliderBase
{
public:
	struct CollisionMesh
	{
		struct Triangle
		{
			Vector3	positions[3];
			Vector3	normal;
		};
		struct Area
		{
			DirectX::BoundingBox	boundingBox;
			std::vector<int>		triangleIndices;
		};

		std::vector<Triangle>	triangles;
		std::vector<Area>		areas;
	};

public:
	MeshCollider() {}
	~MeshCollider() override {}
	// 名前取得
	const char* GetName() const override { return "MeshCollider"; }
	// 開始処理
	void Start() override;
	// 削除時処理
	void OnDelete() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// デバッグ描画処理
	void DebugRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;
	// コリジョンメッシュの再計算
	virtual CollisionMesh RecalculateCollisionMesh(Model* model) const;

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
		float* distance,
		Vector3* hitPosition,
		Vector3* hitNormal);
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
		float* distance/*in out*/,
		Vector3* hitPosition/*out*/,
		Vector3* hitNormal/*out*/);
#pragma region アクセサ
	// コリジョンメッシュ取得
	const CollisionMesh& GetCollisionMesh() {
		std::lock_guard<std::mutex> lock(_collisionMeshMutex); 
		return _collisionMesh;
	}
	// 再計算フラグのセット
	void SetRecalculate(bool f) { _recalculate = f; }
#pragma endregion
protected:
    /// <summary>
    /// 指定のAABBとコリジョンメッシュのAABBの交差判定
    /// </summary>
    /// <param name="aabb"></param>
    /// <returns>交差したAABBの番号</returns>
    std::vector<size_t> GetCollisionMeshIndex(const CollisionMesh& collisionMesh, const DirectX::BoundingBox& aabb) const;
protected:
	CollisionMesh	_collisionMesh;	// コリジョンメッシュ
	bool _recalculate = true;	// 再計算フラグ
	bool _isDebugDrawVertex = false; // 頂点描画フラグ
	bool _isDebugDrawArea = false; // 頂点描画フラグ
	int _cellSize = 16; // 分割エリアのサイズ
    int _drawCellIndex = -1; // 描画エリアのインデックス

	std::mutex _collisionMeshMutex; // マルチスレッド用のミューテックス
};