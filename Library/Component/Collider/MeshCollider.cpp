#include "MeshCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"
#include "../../DebugSupporter/DebugSupporter.h"

// 開始処理
void MeshCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterMeshCollider(this);
	_recalculate = true;
}

// 削除時処理
void MeshCollider::Deleted()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterMeshCollider(this);
}

// 更新処理
void MeshCollider::Update(float elapsedTime)
{
	if (_recalculate)
	{
		_recalculate = false;
		// コリジョンメッシュの計算
		RecalculateCollisionMesh();
	}
}

// デバッグ描画処理
void MeshCollider::DebugRender(const RenderContext& rc)
{
	// デバッグ描画フラグがオフなら何もしない
	if (!GetActor()->IsDrawingDebug())return;

	// 有効でなければ描画しない
	if (!IsActive())
		return;

	for (const auto& area : _collisionMesh.areas)
	{
        Vector3 extents = area.boundingBox.Extents;
        // extents.yが無限大で表示ができなくなるので適当に補正
		extents.y = 10.0f;
		Debug::Renderer::DrawBox(area.boundingBox.Center, Vector3::Zero, extents, Vector4::Yellow);
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
    ImGui::SliderInt(u8"描画エリアインデックス", &_drawCellIndex, -1, _cellSize * _cellSize - 1);
	if (ImGui::Button(u8"コリジョンメッシュ再計算"))
	{
		_recalculate = true;
	}
}

// コリジョンメッシュの再計算
void MeshCollider::RecalculateCollisionMesh()
{
	// モデル確認
	if (GetActor()->GetModel().lock() == nullptr)
		return;

	_collisionMesh.areas.clear();
	_collisionMesh.triangles.clear();

	Model* model = GetActor()->GetModel().lock().get();
	ModelResource* modelResource = GetActor()->GetModel().lock()->GetResource();

	DirectX::XMVECTOR VolumeMin = DirectX::XMVectorReplicate(FLT_MAX);
	DirectX::XMVECTOR VolumeMax = DirectX::XMVectorReplicate(-FLT_MAX);

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
			CollisionMesh::Triangle& triangle = _collisionMesh.triangles.emplace_back();
			DirectX::XMStoreFloat3(&triangle.positions[0], A);
			DirectX::XMStoreFloat3(&triangle.positions[1], B);
			DirectX::XMStoreFloat3(&triangle.positions[2], C);	
			DirectX::XMStoreFloat3(&triangle.normal, N);

			// モデル全体のAABBを計測
			VolumeMin = DirectX::XMVectorMin(VolumeMin, A);
			VolumeMin = DirectX::XMVectorMin(VolumeMin, B);
			VolumeMin = DirectX::XMVectorMin(VolumeMin, C);
			VolumeMax = DirectX::XMVectorMax(VolumeMax, A);
			VolumeMax = DirectX::XMVectorMax(VolumeMax, B);
			VolumeMax = DirectX::XMVectorMax(VolumeMax, C);
		}
	}

	// モデル全体のAABB
	DirectX::XMFLOAT3 volumeMin, volumeMax;
	DirectX::XMStoreFloat3(&volumeMin, VolumeMin);
	DirectX::XMStoreFloat3(&volumeMax, VolumeMax);

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

/// 指定のAABBとコリジョンメッシュのAABBの交差判定
std::vector<size_t> MeshCollider::GetCollisionMeshIndex(const DirectX::BoundingBox& aabb)
{
    std::vector<size_t> indices;
	size_t index = 0;
    // コリジョンメッシュのAABBと交差しているエリアを取得
    for (const auto& area : _collisionMesh.areas)
    {
        if (area.boundingBox.Intersects(aabb))
        {
            indices.push_back(index);
        }
		index++;
    }
	return indices;
}
