#include "TerrainCollider.h"

#include "../../Scene/Scene.h"
#include "../../DebugSupporter/DebugSupporter.h"
#include "../../Library/Collision/CollisionMath.h"

#include <imgui.h>

// 生成時処理
void TerrainCollider::OnCreate()
{
	MeshCollider::OnCreate();
	_terrainController = GetActor()->GetComponent<TerrainController>();
	_worldMatrix = &GetActor()->GetTransform().GetMatrix();
	_recalculate = false;
}
// 更新処理
void TerrainCollider::Update(float elapsedTime)
{
	MeshCollider::Update(elapsedTime);
	// 地形コントローラーが有効で、地形編集が完了している場合は再計算
	if (_terrainController.lock() && 
		_terrainController.lock()->GetEditState() == TerrainController::EditState::Complete)
	{
		_recalculate = true;
	}
}
// コリジョンメッシュの再計算
MeshCollider::CollisionMesh TerrainCollider::RecalculateCollisionMesh(Model* model) const
{
	if (!_terrainController.lock())
		return CollisionMesh();
	if (!_worldMatrix)
		return CollisionMesh();
	auto terrain = _terrainController.lock()->GetTerrain().lock();
    if (!terrain)
		return CollisionMesh();
	auto& streamOutData = terrain->GetStreamOutData();
	if (streamOutData.empty())
		return CollisionMesh();

	CollisionMesh collisionMesh;
	Vector3 volumeMin = Vector3::Max;
	Vector3 volumeMax = Vector3::Min;

    // ストリームアウトのデータから三角形を生成
    for (size_t i = 0; i < streamOutData.size(); i += 3)
    {
        Vector3 v1 = streamOutData[i + 0].worldPosition;
		Vector3 v2 = streamOutData[i + 1].worldPosition;
		Vector3 v3 = streamOutData[i + 2].worldPosition;

		// 法線ベクトルを算出
		Vector3 n = Vector3::Cross(v2 - v1, v3 - v1);

		// 三角形データを格納
		CollisionMesh::Triangle& triangle = collisionMesh.triangles.emplace_back();
		triangle.positions[0] = v1;
		triangle.positions[1] = v2;
		triangle.positions[2] = v3;
		triangle.normal = n.Normalize();

		// モデル全体のAABBを計測
		volumeMin = volumeMin.Minimum(v1);
		volumeMin = volumeMin.Minimum(v2);
		volumeMin = volumeMin.Minimum(v3);
		volumeMax = volumeMax.Maximum(v1);
		volumeMax = volumeMax.Maximum(v2);
		volumeMax = volumeMax.Maximum(v3);
    }
    // 透明壁のデータから三角形を生成
    for (size_t i = 0; i < terrain->GetTransparentWall()->GetWalls().size(); i++)
    {
        auto& wall = terrain->GetTransparentWall()->GetWalls()[i];
		size_t pointCount = wall.vertices.size();
		if (pointCount <= 1)
			continue;
		Vector3 heightOffset = Vector3(0.0f, wall.height, 0.0f);
		for (size_t i = 0; i < pointCount - 1; i++)
		{
			const Vector3& p1 = wall.vertices[i].TransformCoord(*_worldMatrix);
			const Vector3& p2 = wall.vertices[i + 1].TransformCoord(*_worldMatrix);
			const Vector3& p3 = p1 + heightOffset;
			const Vector3& p4 = p2 + heightOffset;

			// 法線ベクトルを算出
			Vector3 normal = (p2 - p1).Cross(p3 - p1).Normalize();
			// 三角形データを格納
			CollisionMesh::Triangle& triangle0 = collisionMesh.triangles.emplace_back();
			triangle0.positions[0] = p1;
			triangle0.positions[1] = p2;
			triangle0.positions[2] = p3;
			triangle0.normal = normal;
			CollisionMesh::Triangle& triangle1 = collisionMesh.triangles.emplace_back();
			triangle1.positions[0] = p2;
			triangle1.positions[1] = p4;
			triangle1.positions[2] = p3;
			triangle1.normal = normal;
		}
    }

	// AABBの構築
	BuildCollisionMeshAABB(collisionMesh, volumeMin, volumeMax);

	return collisionMesh;
}
