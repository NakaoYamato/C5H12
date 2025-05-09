#include "CollisionManager.h"

#include <imgui.h>

#include "../DebugSupporter/DebugSupporter.h"

void CollisionManager::Update()
{
}

void CollisionManager::DebugRender(const RenderContext& rc)
{
	// デバッグ描画フラグがオフなら何もしない
	if (!_isDebugDraw)return;

	// 球コライダーのデバッグ描画
	for (auto& sphereCollider : _sphereColliders)
	{
		Debug::Renderer::DrawSphere(
			sphereCollider->GetPosition().TransformCoord(sphereCollider->GetActor()->GetTransform().GetMatrix()),
			sphereCollider->GetRadius(),
			_VECTOR4_RED);
	}
	// ボックスコライダーのデバッグ描画
	for (auto& boxCollider : _boxColliders)
	{
		Debug::Renderer::DrawBox(
			boxCollider->GetPosition().TransformCoord(boxCollider->GetActor()->GetTransform().GetMatrix()),
			boxCollider->GetActor()->GetTransform().GetRotation(),
			boxCollider->GetHalfSize(), 
			_VECTOR4_GREEN);
	}
	// カプセルコライダーのデバッグ描画
	for (auto& capsuleCollider : _capsuleColliders)
	{
		Debug::Renderer::DrawCapsule(
			capsuleCollider->GetStart().TransformCoord(capsuleCollider->GetActor()->GetTransform().GetMatrix()),
			capsuleCollider->GetEnd().TransformCoord(capsuleCollider->GetActor()->GetTransform().GetMatrix()),
			capsuleCollider->GetRadius(),
			_VECTOR4_BLUE);
	}
	// メッシュコライダーのデバッグ描画
	for (auto& meshCollider : _meshColliders)
	{
		const auto& collisionMesh = meshCollider->GetCollisionMesh();
		for (const auto& area : collisionMesh.areas)
		{
			Debug::Renderer::DrawBox(area.boundingBox.Center, _VECTOR3_ZERO, area.boundingBox.Extents, _VECTOR4_YELLOW);
		}
		if (_isDebugDrawVertex)
		{
			for (const auto& triangle : collisionMesh.triangles)
			{
				Debug::Renderer::AddVertex(triangle.positions[0]);
				Debug::Renderer::AddVertex(triangle.positions[1]);
				Debug::Renderer::AddVertex(triangle.positions[1]);
				Debug::Renderer::AddVertex(triangle.positions[2]);
				Debug::Renderer::AddVertex(triangle.positions[2]);
				Debug::Renderer::AddVertex(triangle.positions[0]);
			}
		}
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
				ImGui::Checkbox(u8"デバッグ描画", &_isDebugDraw);

				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

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
