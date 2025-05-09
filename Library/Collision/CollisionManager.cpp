#include "CollisionManager.h"

#include <imgui.h>

#include "CollisionMath.h"
#include "../DebugSupporter/DebugSupporter.h"

void CollisionManager::Update()
{
	Vector3 hitPosition{};
	Vector3 hitNormal{};
	float penetration = 0.0f;

	// 

	// 球の当たり判定
	for (auto& sphereA : _sphereColliders)
	{
		Transform& transformA = sphereA->GetActor()->GetTransform();

		// 球Vs球
		for (auto& sphereB : _sphereColliders)
		{
			// 同じ場合は処理しない
			if (sphereA == sphereB)
				continue;

			Transform& transformB = sphereB->GetActor()->GetTransform();
			if (Collision3D::IntersectSphereVsSphere(
				sphereA->GetPosition().TransformCoord(transformA.GetMatrix()),
				sphereA->GetRadius(),
				sphereB->GetPosition().TransformCoord(transformB.GetMatrix()),
				sphereB->GetRadius(),
				hitPosition,
				hitNormal,
				penetration))
			{
				// トリガーでなければ押し出し処理
				if (!sphereA->IsTrigger())
				{
					sphereA->GetActor()->GetTransform().SetPosition(
						sphereA->GetActor()->GetTransform().GetPosition() + hitNormal * penetration);
				}
				sphereA->GetActor()->OnCollision(
					sphereB->GetActor().get(),
					hitPosition,
					hitNormal,
					penetration);

				if (!sphereB->IsTrigger())
				{
					sphereB->GetActor()->GetTransform().SetPosition(
						sphereB->GetActor()->GetTransform().GetPosition() - hitNormal * penetration);
				}
				sphereB->GetActor()->OnCollision(
					sphereA->GetActor().get(),
					hitPosition,
					-hitNormal,
					penetration);
			}
		}

		//for (auto& boxCollider : _boxColliders)
		//{
		//	Collision3D::IntersectSphereVsAABB(
		//		sphereCollider->GetPosition(),
		//		sphereCollider->GetRadius(),
		//		boxCollider->GetPosition(),
		//		boxCollider->GetSize());
		//}
	}
}

void CollisionManager::DrawGui()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			if (ImGui::BeginMenu(u8"当たり判定"))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

#pragma region キャスト
/// レイキャスト
bool CollisionManager::RayCast(
	const Vector3& start,
	const Vector3& direction,
	float& distance,
	Vector3& hitPosition,
	Vector3& hitNormal)
{
	bool hit = false;
	DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR DirectionNorm = DirectX::XMLoadFloat3(&direction);

	// 各メッシュコライダーに対してレイキャストを行う
	for (auto& meshCollider : _meshColliders)
	{
		for (auto& area : meshCollider->GetCollisionMesh().areas)
		{
			DirectX::XMVECTOR aabbCenter = DirectX::XMLoadFloat3(&area.boundingBox.Center);
			DirectX::XMVECTOR aabbRadii = DirectX::XMLoadFloat3(&area.boundingBox.Extents);
			// レイVsAABB
			if (Collision3D::IntersectRayVsAABB(Start, DirectionNorm, distance, aabbCenter, aabbRadii, nullptr, nullptr))
			{
				HitResult tmpResult;
				// エリアに含まれている三角形と判定
				for (const int& index : area.triangleIndices)
				{
					const MeshCollider::CollisionMesh::Triangle& triangle = meshCollider->GetCollisionMesh().triangles[index];
					DirectX::XMVECTOR TrianglePos[3] = {
						DirectX::XMLoadFloat3(&triangle.positions[0]),
						DirectX::XMLoadFloat3(&triangle.positions[1]),
						DirectX::XMLoadFloat3(&triangle.positions[2])
					};

					// レイVs三角形
					if (Collision3D::IntersectRayVsTriangle(Start, DirectionNorm, distance, TrianglePos, tmpResult))
					{
						// 最近距離を更新
						if (distance > tmpResult.distance)
						{
							hitPosition		= tmpResult.position;
							hitNormal		= tmpResult.normal;
							distance		= tmpResult.distance;
						}
						hit = true;
					}
				}
			}
		}
	}

	return hit;
}

/// スフィアキャスト
bool CollisionManager::SphereCast(const Vector3& origin, const Vector3& direction, float radius, float& distance, Vector3& hitPosition, Vector3& hitNormal)
{
	bool hit = false;
	DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&origin);
	DirectX::XMVECTOR DirectionNorm = DirectX::XMLoadFloat3(&direction);

	// 各メッシュコライダーに対してスフィアキャスト行う
	for (auto& meshCollider : _meshColliders)
	{
		for (auto& area : meshCollider->GetCollisionMesh().areas)
		{
			DirectX::XMVECTOR slubCenter = DirectX::XMLoadFloat3(&area.boundingBox.Center);
			DirectX::XMVECTOR slubRadii = DirectX::XMLoadFloat3(&area.boundingBox.Extents);
			// スフィアキャストVSAABB
			if (Collision3D::IntersectCapsuleVsAABB(Start, DirectionNorm, distance, radius, slubCenter, slubRadii))
			{
				HitResult tmpResult;
				// エリアに含まれている三角形と判定
				for (const int& index : area.triangleIndices)
				{
					const MeshCollider::CollisionMesh::Triangle& triangle = meshCollider->GetCollisionMesh().triangles[index];
					DirectX::XMVECTOR TrianglePos[3] = {
						DirectX::XMLoadFloat3(&triangle.positions[0]),
						DirectX::XMLoadFloat3(&triangle.positions[1]),
						DirectX::XMLoadFloat3(&triangle.positions[2])
					};

					// スフィアキャストVs三角形
					if (Collision3D::IntersectSphereCastVsTriangle(Start, DirectionNorm, distance, radius, TrianglePos, &tmpResult, false))
					{
						// 最近距離を更新
						if (distance > tmpResult.distance)
						{
							hitPosition = tmpResult.position;
							hitNormal = tmpResult.normal;
							distance = tmpResult.distance;
						}
						hit = true;
					}
				}
			}
		}
	}

	return hit;
}
#pragma endregion

#pragma region 登録
// 球コライダー登録
void CollisionManager::RegisterSphereCollider(SphereCollider* sphereCollider)
{
	_sphereColliders.push_back(sphereCollider);
}
// ボックスコライダー登録	
void CollisionManager::RegisterBoxCollider(BoxCollider* boxCollider)
{
	_boxColliders.push_back(boxCollider);
}
// カプセルコライダー登録
void CollisionManager::RegisterCapsuleCollider(CapsuleCollider* capsuleCollider)
{
	_capsuleColliders.push_back(capsuleCollider);
}
// メッシュコライダー登録
void CollisionManager::RegisterMeshCollider(MeshCollider* meshCollider)
{
	_meshColliders.push_back(meshCollider);
}

#pragma endregion

#pragma region 削除
// 球コライダー削除
void CollisionManager::UnregisterSphereCollider(SphereCollider* sphereCollider)
{
	auto it = std::find(_sphereColliders.begin(), _sphereColliders.end(), sphereCollider);
	if (it != _sphereColliders.end())
	{
		_sphereColliders.erase(it);
	}
	else
	{
		Debug::Output::String(u8"球コライダーの削除に失敗しました");
	}
}
// ボックスコライダー削除
void CollisionManager::UnregisterBoxCollider(BoxCollider* boxCollider)
{
	auto it = std::find(_boxColliders.begin(), _boxColliders.end(), boxCollider);
	if (it != _boxColliders.end())
	{
		_boxColliders.erase(it);
	}
	else
	{
		Debug::Output::String(u8"ボックスコライダーの削除に失敗しました");
	}
}
// カプセルコライダー削除
void CollisionManager::UnregisterCapsuleCollider(CapsuleCollider* capsuleCollider)
{
	auto it = std::find(_capsuleColliders.begin(), _capsuleColliders.end(), capsuleCollider);
	if (it != _capsuleColliders.end())
	{
		_capsuleColliders.erase(it);
	}
	else
	{
		Debug::Output::String(u8"カプセルコライダーの削除に失敗しました");
	}
}
void CollisionManager::UnregisterMeshCollider(MeshCollider* meshCollider)
{
	auto it = std::find(_meshColliders.begin(), _meshColliders.end(), meshCollider);
	if (it != _meshColliders.end())
	{
		_meshColliders.erase(it);
	}
	else
	{
		Debug::Output::String(u8"メッシュコライダーの削除に失敗しました");
	}
}

#pragma endregion
