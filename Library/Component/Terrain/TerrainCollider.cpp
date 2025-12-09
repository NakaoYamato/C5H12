#include "TerrainCollider.h"

#include "../../Scene/Scene.h"
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
	//// 地形コントローラーが有効で、地形編集が完了している場合は再計算
	//if (_terrainController.lock() && 
	//	_terrainController.lock()->GetEditState() == TerrainController::EditState::Complete)
	//{
	//	_recalculate = true;
	//}
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

	// AABBの構築
	BuildCollisionMeshAABB(collisionMesh, volumeMin, volumeMax);

	return collisionMesh;
}
