#include "TransparentWallCollider.h"

#include <imgui.h>

// コンストラクタ
TransparentWallCollider::TransparentWallCollider(std::string filepath) :
	_filepath(filepath)
{
	// 透明壁の読み込み
	_transparentWall.LoadFromFile(_filepath);
}

// 更新処理
void TransparentWallCollider::Update(float elapsedTime)
{
}

// デバッグ描画処理
void TransparentWallCollider::DebugRender(const RenderContext& rc)
{
}

// GUI描画
void TransparentWallCollider::DrawGui()
{
	_transparentWall.DrawGui();
}

// コリジョンメッシュの再計算
MeshCollider::CollisionMesh TransparentWallCollider::RecalculateCollisionMesh(Model* model) const
{
	CollisionMesh collisionMesh;
	Vector3 volumeMin = Vector3::Max;
	Vector3 volumeMax = Vector3::Min;

	// 透明壁のデータから三角形を生成
	for (size_t i = 0; i < _transparentWall.GetWalls().size(); i++)
	{
		auto& wall = _transparentWall.GetWalls()[i];
		size_t pointCount = wall.vertices.size();
		if (pointCount <= 1)
			continue;
		Vector3 heightOffset = Vector3(0.0f, wall.height, 0.0f);
		for (size_t i = 0; i < pointCount - 1; i++)
		{
			const Vector3& p1 = wall.vertices[i];
			const Vector3& p2 = wall.vertices[i + 1];
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
