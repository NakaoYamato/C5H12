#include "MeshCollider.h"

#include "../../Scene/Scene.h"
#include "../../DebugSupporter/DebugSupporter.h"
#include "../../Library/Collision/CollisionMath.h"
#include "../../Library/JobSystem/JobSystem.h"

#include <imgui.h>

// 開始処理
void MeshCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterMeshCollider(this);
	_recalculate = true;
}
// 削除時処理
void MeshCollider::OnDelete()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterMeshCollider(this);
}
// 更新処理
void MeshCollider::Update(float elapsedTime)
{
	if (_recalculate && !_isCalculating)
	{
		_recalculate = false;

		// 再計算中フラグを立てる
		_isCalculating = true;
		// コリジョンメッシュの再計算をジョブシステムに申請
		JobSystem::Instance().EnqueueJob(
			GetName(),
			ImGuiControl::Profiler::Color::Green,
			[&]()
			{
				CollisionMesh collisionMeshCopy = RecalculateCollisionMesh(GetActor()->GetModel().lock().get());
				{
					std::lock_guard<std::mutex> lock(_collisionMeshMutex);
					_calcCollisionMesh = collisionMeshCopy;
					_isCalculating = false;
				}
			});
	}

	{
		std::lock_guard<std::mutex> lock(_collisionMeshMutex);
		// コリジョンメッシュの更新
		if (!_calcCollisionMesh.areas.empty() && !_calcCollisionMesh.triangles.empty())
		{
			_collisionMesh = _calcCollisionMesh;
			_calcCollisionMesh.areas.clear();
			_calcCollisionMesh.triangles.clear();
		}
	}
}
// デバッグ描画処理
void MeshCollider::DebugRender(const RenderContext& rc)
{
	// アクターが描画階層を描画していない場合は何もしない
	if (!GetActor()->IsDrawingHierarchy())return;

	// 有効でなければ描画しない
	if (!IsActive())
		return;

	std::lock_guard<std::mutex> lock(_collisionMeshMutex);

	if (_isDebugDrawArea)
	{
		for (const auto& area : _collisionMesh.areas)
		{
			Vector3 extents = area.boundingBox.Extents;
			// extents.yが無限大で表示ができなくなるので適当に補正
			extents.y = 10.0f;
			Debug::Renderer::DrawBox(area.boundingBox.Center, Vector3::Zero, extents, Vector4::Yellow);
		}
	}
	if (_isDebugDrawVertex)
	{
		for (const auto& triangle : _collisionMesh.triangles)
		{
			Debug::Renderer::AddVertex(triangle.positions[0]);
			Debug::Renderer::AddVertex(triangle.positions[1]);
			Debug::Renderer::AddVertex(triangle.positions[1]);
			Debug::Renderer::AddVertex(triangle.positions[2]);
			Debug::Renderer::AddVertex(triangle.positions[2]);
			Debug::Renderer::AddVertex(triangle.positions[0]);
		}
	}
	if (_drawCellIndex != -1)
	{
		const auto& area = _collisionMesh.areas[_drawCellIndex];
        for (const auto& triangle : area.triangleIndices)
        {
            Debug::Renderer::AddVertex(_collisionMesh.triangles[triangle].positions[0]);
            Debug::Renderer::AddVertex(_collisionMesh.triangles[triangle].positions[1]);
            Debug::Renderer::AddVertex(_collisionMesh.triangles[triangle].positions[1]);
            Debug::Renderer::AddVertex(_collisionMesh.triangles[triangle].positions[2]);
            Debug::Renderer::AddVertex(_collisionMesh.triangles[triangle].positions[2]);
            Debug::Renderer::AddVertex(_collisionMesh.triangles[triangle].positions[0]);
        }
	}
}
// GUI描画
void MeshCollider::DrawGui()
{
	ColliderBase::DrawGui();
	ImGui::DragInt(u8"分割エリアサイズ", &_cellSize, 1.0f, 1, 32);
	ImGui::Checkbox(u8"コリジョンメッシュ頂点表示", &_isDebugDrawVertex);
	ImGui::Checkbox(u8"コリジョンメッシュエリア表示", &_isDebugDrawArea);
    ImGui::SliderInt(u8"描画エリアインデックス", &_drawCellIndex, -1, _cellSize * _cellSize - 1);
	if (ImGui::Button(u8"コリジョンメッシュ再計算"))
	{
		_recalculate = true;
	}
}
// コリジョンメッシュの再計算
MeshCollider::CollisionMesh MeshCollider::RecalculateCollisionMesh(Model* model) const
{
	// モデル確認
	if (model == nullptr)
		return CollisionMesh();

	CollisionMesh collisionMesh;
	ModelResource* modelResource = model->GetResource();
	Vector3 volumeMin = Vector3::Max;
	Vector3 volumeMax = Vector3::Min;

	// 頂点データをワールド空間変換し、三角形データを作成
	for (const ModelResource::Mesh& mesh : modelResource->GetMeshes())
	{
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&model->GetPoseNodes().at(mesh.nodeIndex).worldTransform);
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			// 頂点データをワールド空間変換
			//uint32_t a = mesh.indices.at(i + 0);
			//uint32_t b = mesh.indices.at(i + 1);
			//uint32_t c = mesh.indices.at(i + 2);
			// TODO : なぜか逆向き
			uint32_t a = mesh.indices.at(i + 2);
			uint32_t b = mesh.indices.at(i + 1);
			uint32_t c = mesh.indices.at(i + 0);
			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&mesh.vertices.at(a).position);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&mesh.vertices.at(b).position);
			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&mesh.vertices.at(c).position);
			A = DirectX::XMVector3Transform(A, WorldTransform);
			B = DirectX::XMVector3Transform(B, WorldTransform);
			C = DirectX::XMVector3Transform(C, WorldTransform);

			// 法線ベクトルを算出
			DirectX::XMVECTOR N = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A));
			if (DirectX::XMVector3Equal(N, DirectX::XMVectorZero()))
			{
				// 面を構成できない場合は除外
				continue;
			}
			N = DirectX::XMVector3Normalize(N);

			// 三角形データを格納
			CollisionMesh::Triangle& triangle = collisionMesh.triangles.emplace_back();
			DirectX::XMStoreFloat3(&triangle.positions[0], A);
			DirectX::XMStoreFloat3(&triangle.positions[1], B);
			DirectX::XMStoreFloat3(&triangle.positions[2], C);	
			DirectX::XMStoreFloat3(&triangle.normal, N);

			// モデル全体のAABBを計測
			volumeMin = volumeMin.Minimum(triangle.positions[0]);
			volumeMin = volumeMin.Minimum(triangle.positions[1]);
			volumeMin = volumeMin.Minimum(triangle.positions[2]);
			volumeMax = volumeMax.Maximum(triangle.positions[0]);
			volumeMax = volumeMax.Maximum(triangle.positions[1]);
			volumeMax = volumeMax.Maximum(triangle.positions[2]);
		}
	}

	// AABBの構築
	BuildCollisionMeshAABB(collisionMesh, volumeMin, volumeMax);

	return collisionMesh;
}
/// レイキャスト
bool MeshCollider::RayCast(
	const Vector3& start,
	const Vector3& direction, 
	float* distance,
	Vector3* hitPosition,
	Vector3* hitNormal)
{
	bool hit = false;
	DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR DirectionNorm = DirectX::XMLoadFloat3(&direction);
	auto& collisionMesh = GetCollisionMesh();

	// 全体のAABBとレイの交差判定
	{
		DirectX::XMVECTOR aabbCenter = DirectX::XMLoadFloat3(&collisionMesh.meshBoundingBox.Center);
		DirectX::XMVECTOR aabbRadii = DirectX::XMLoadFloat3(&collisionMesh.meshBoundingBox.Extents);
		if (!Collision3D::IntersectRayVsAABB(Start, DirectionNorm, *distance, aabbCenter, aabbRadii, nullptr, nullptr))
		{
			return false;
		}
	}

	for (auto& area : collisionMesh.areas)
	{
		DirectX::XMVECTOR aabbCenter = DirectX::XMLoadFloat3(&area.boundingBox.Center);
		DirectX::XMVECTOR aabbRadii = DirectX::XMLoadFloat3(&area.boundingBox.Extents);
		// レイVsAABB
		if (Collision3D::IntersectRayVsAABB(Start, DirectionNorm, *distance, aabbCenter, aabbRadii, nullptr, nullptr))
		{
			HitResult tmpResult;
			// エリアに含まれている三角形と判定
			for (const int& index : area.triangleIndices)
			{
				const MeshCollider::CollisionMesh::Triangle& triangle = collisionMesh.triangles[index];
				DirectX::XMVECTOR TrianglePos[3] = {
					DirectX::XMLoadFloat3(&triangle.positions[0]),
					DirectX::XMLoadFloat3(&triangle.positions[1]),
					DirectX::XMLoadFloat3(&triangle.positions[2])
				};

				// レイVs三角形
				if (Collision3D::IntersectRayVsTriangle(Start, DirectionNorm, *distance, TrianglePos, tmpResult))
				{
					// 最近距離を更新
					if (*distance > tmpResult.distance)
					{
						*hitPosition = tmpResult.position;
						*hitNormal = tmpResult.normal;
						*distance = tmpResult.distance;
					}
					hit = true;
				}
			}
		}
	}

	return hit;
}
/// スフィアキャスト
bool MeshCollider::SphereCast(const Vector3& origin, const Vector3& direction, float radius, float* distance, Vector3* hitPosition, Vector3* hitNormal)
{
	bool hit = false;
	DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&origin);
	DirectX::XMVECTOR DirectionNorm = DirectX::XMLoadFloat3(&direction);
	auto& collisionMesh = GetCollisionMesh();

	// 全体のAABBとレイの交差判定
	{
		DirectX::XMVECTOR aabbCenter = DirectX::XMLoadFloat3(&collisionMesh.meshBoundingBox.Center);
		DirectX::XMVECTOR aabbRadii = DirectX::XMLoadFloat3(&collisionMesh.meshBoundingBox.Extents);
		if (!Collision3D::IntersectRayVsAABB(Start, DirectionNorm, *distance, aabbCenter, aabbRadii, nullptr, nullptr))
		{
			return false;
		}
	}

	for (auto& area : collisionMesh.areas)
	{
		DirectX::XMVECTOR slubCenter = DirectX::XMLoadFloat3(&area.boundingBox.Center);
		DirectX::XMVECTOR slubRadii = DirectX::XMLoadFloat3(&area.boundingBox.Extents);
		// スフィアキャストVSAABB
		if (Collision3D::IntersectCapsuleVsAABB(Start, DirectionNorm, *distance, radius, slubCenter, slubRadii))
		{
			HitResult tmpResult;
			// エリアに含まれている三角形と判定
			for (const int& index : area.triangleIndices)
			{
				const MeshCollider::CollisionMesh::Triangle& triangle = collisionMesh.triangles[index];
				DirectX::XMVECTOR TrianglePos[3] = {
					DirectX::XMLoadFloat3(&triangle.positions[0]),
					DirectX::XMLoadFloat3(&triangle.positions[1]),
					DirectX::XMLoadFloat3(&triangle.positions[2])
				};

				// スフィアキャストVs三角形
				if (Collision3D::IntersectSphereCastVsTriangle(Start, DirectionNorm, *distance, radius, TrianglePos, &tmpResult, false))
				{
					// 最近距離を更新
					if (*distance > tmpResult.distance)
					{
						*hitPosition = tmpResult.position;
						*hitNormal = tmpResult.normal;
						*distance = tmpResult.distance;
					}
					hit = true;
				}
			}
		}
	}

	return hit;
}
/// コリジョンメッシュのAABBの構築
void MeshCollider::BuildCollisionMeshAABB(CollisionMesh& collisionMesh, const Vector3& volumeMin, const Vector3& volumeMax) const
{
	// モデル全体のAABB
	collisionMesh.meshBoundingBox.Center = (volumeMin + volumeMax) * 0.5f;
	collisionMesh.meshBoundingBox.Extents = Vector3::Maximum((volumeMax - volumeMin), Vector3(0.01f, 0.01f, 0.01f)) * 0.5f;

	collisionMesh.areas.resize((size_t)(_cellSize * _cellSize));
	float sizeX = (volumeMax.x - volumeMin.x) / _cellSize;
	float sizeZ = (volumeMax.z - volumeMin.z) / _cellSize;
	for (int i = 0; i < _cellSize * _cellSize; ++i)
	{
		collisionMesh.areas[i].boundingBox.Center.x = volumeMin.x + sizeX * (float)(i % _cellSize) + 0.5f * sizeX;
		collisionMesh.areas[i].boundingBox.Center.y = 0.0f;
		collisionMesh.areas[i].boundingBox.Center.z = volumeMin.z + sizeZ * (float)(i / _cellSize) + 0.5f * sizeZ;
		collisionMesh.areas[i].boundingBox.Extents.x = 0.5f * sizeX;
		collisionMesh.areas[i].boundingBox.Extents.y = FLT_MAX; // 無限
		collisionMesh.areas[i].boundingBox.Extents.z = 0.5f * sizeZ;
	}
	for (int i = 0; i < collisionMesh.triangles.size(); ++i)
	{
		const CollisionMesh::Triangle& triangle = collisionMesh.triangles[i];
		std::vector<size_t> indexMap;
		// 各頂点がどのエリアに属するかを調べる
		// 三角形からAABBを算出し、各エリアのAABBと衝突しているか調べる
		const Vector3 points[3] = { triangle.positions[0], triangle.positions[1], triangle.positions[2] };
		DirectX::BoundingBox triangleBox;
		DirectX::BoundingBox::CreateFromPoints(triangleBox, 3, points, sizeof(Vector3));
		indexMap = GetCollisionMeshIndex(collisionMesh, triangleBox);
		for (auto& index : indexMap)
		{
			collisionMesh.areas[index].triangleIndices.push_back(i);
		}
	}
}
/// 指定のAABBとコリジョンメッシュのAABBの交差判定
std::vector<size_t> MeshCollider::GetCollisionMeshIndex(const CollisionMesh& collisionMesh, const DirectX::BoundingBox& aabb) const
{
    std::vector<size_t> indices;
	size_t index = 0;
    // コリジョンメッシュのAABBと交差しているエリアを取得
    for (const auto& area : collisionMesh.areas)
    {
        if (area.boundingBox.Intersects(aabb))
        {
            indices.push_back(index);
        }
		index++;
    }
	return indices;
}
