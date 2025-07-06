#include "TerrainCollider.h"

#include "../../Scene/Scene.h"
#include "../../DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 開始処理
void TerrainCollider::Start()
{
	MeshCollider::Start();
	_terrainController = GetActor()->GetComponent<TerrainController>();
}
// 更新処理
void TerrainCollider::Update(float elapsedTime)
{
	// 地形コントローラーが有効で、地形編集フラグが立っている場合は再計算
	if (_terrainController.lock() && _terrainController.lock()->IsEditing())
	{
		_recalculate = true;
	}
	MeshCollider::Update(elapsedTime);
}
// コリジョンメッシュの再計算
void TerrainCollider::RecalculateCollisionMesh()
{
	if (!_terrainController.lock())
		return;
	auto terrain = _terrainController.lock()->GetTerrain().lock();
    if (!terrain)
        return;
	auto& streamOutData = terrain->GetStreamOutData();
	if (streamOutData.empty())
		return;

	_collisionMesh.areas.clear();
	_collisionMesh.triangles.clear();

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
		CollisionMesh::Triangle& triangle = _collisionMesh.triangles.emplace_back();
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
    for (size_t i = 0; i < terrain->GetTransparentWalls().size(); i++)
    {
        auto& wall = terrain->GetTransparentWalls()[i];
		size_t pointCount = wall.points.size();
		if (pointCount <= 1)
			continue;
		Vector3 heightOffset = Vector3(0.0f, wall.height, 0.0f);
		for (size_t i = 0; i < pointCount - 1; i++)
		{
			const Vector3& p1 = wall.points[i];
			const Vector3& p2 = wall.points[i + 1];
			const Vector3& p3 = wall.points[i] + heightOffset;
			const Vector3& p4 = wall.points[i + 1] + heightOffset;

			// 法線ベクトルを算出
			Vector3 normal = (p2 - p1).Cross(p3 - p1).Normalize();
			// 三角形データを格納
			CollisionMesh::Triangle& triangle0 = _collisionMesh.triangles.emplace_back();
			triangle0.positions[0] = p1;
			triangle0.positions[1] = p2;
			triangle0.positions[2] = p3;
			triangle0.normal = normal;
			CollisionMesh::Triangle& triangle1 = _collisionMesh.triangles.emplace_back();
			triangle1.positions[0] = p2;
			triangle1.positions[1] = p4;
			triangle1.positions[2] = p3;
			triangle1.normal = normal;
		}
    }


	// モデル全体のAABB
	_collisionMesh.areas.resize((size_t)(_cellSize * _cellSize));
	float sizeX = (volumeMax.x - volumeMin.x) / _cellSize;
	float sizeZ = (volumeMax.z - volumeMin.z) / _cellSize;
	for (int i = 0; i < _cellSize * _cellSize; ++i)
	{
		_collisionMesh.areas[i].boundingBox.Center.x = volumeMin.x + sizeX * (float)(i % _cellSize) + 0.5f * sizeX;
		_collisionMesh.areas[i].boundingBox.Center.y = 0.0f;
		_collisionMesh.areas[i].boundingBox.Center.z = volumeMin.z + sizeZ * (float)(i / _cellSize) + 0.5f * sizeZ;
		_collisionMesh.areas[i].boundingBox.Extents.x = 0.5f * sizeX;
		_collisionMesh.areas[i].boundingBox.Extents.y = FLT_MAX; // 無限
		_collisionMesh.areas[i].boundingBox.Extents.z = 0.5f * sizeZ;
	}
	for (int i = 0; i < _collisionMesh.triangles.size(); ++i)
	{
		const CollisionMesh::Triangle& triangle = _collisionMesh.triangles[i];
		std::vector<size_t> indexMap;
		// 各頂点がどのエリアに属するかを調べる
		// 三角形からAABBを算出し、各エリアのAABBと衝突しているか調べる
		const Vector3 points[3] = { triangle.positions[0], triangle.positions[1], triangle.positions[2] };
		DirectX::BoundingBox triangleBox;
		DirectX::BoundingBox::CreateFromPoints(triangleBox, 3, points, sizeof(Vector3));
		indexMap = GetCollisionMeshIndex(triangleBox);
		for (auto& index : indexMap)
		{
			_collisionMesh.areas[index].triangleIndices.push_back(i);
		}
	}
}
