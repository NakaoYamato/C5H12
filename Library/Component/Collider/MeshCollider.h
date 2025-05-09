#pragma once

#include <DirectXCollision.h>
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
	void OnDestroy() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// デバッグ描画処理
	void DebugRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;
	// コリジョンメッシュの再計算
	void RecalculateCollisionMesh();
#pragma region アクセサ
	// コリジョンメッシュ取得
	const CollisionMesh& GetCollisionMesh() const { return _collisionMesh; }
#pragma endregion
private:
	// 座標を分割エリアのkeyに変換
	size_t CalcKey(const Vector3& position,
		const float& sizeX, const float& sizeZ,
		const float& minX, const float& minZ,
		const size_t& sellsize);
private:
	CollisionMesh	_collisionMesh;	// コリジョンメッシュ
	bool _recalculate = true;	// 再計算フラグ
	bool _isDebugDrawVertex = false; // 頂点描画フラグ
	int _cellSize = 16; // 分割エリアのサイズ
};