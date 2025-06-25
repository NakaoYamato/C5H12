#include "CollisionManager.h"

#include <imgui.h>

#include "CollisionMath.h"
#include "../../Library/JobSystem/JobSystem.h"
#include "../DebugSupporter/DebugSupporter.h"

void CollisionManager::Update()
{
	// 衝突情報のコンテナ
    std::unordered_map<Actor*, std::vector<CollisionData>> collisionDataMap;

	// 各コンポーネントの情報を設定
	SetDataByCollider();

	// 全体の計算時間
	ProfileScopedSection_2(0, ActorManager, ImGuiControl::Profiler::Purple);
    // ジョブシステムでマルチスレッドを使用する場合
	if (JobSystem::Instance().UseMultiThread())
	{
		std::vector<std::future<void>> jobResults;

		// 球の当たり判定
		for (size_t sphereIndex = 0; sphereIndex < _sphereDatas.size(); ++sphereIndex)
		{
			auto& sphereA = _sphereDatas[sphereIndex];
			jobResults.emplace_back(JobSystem::Instance().EnqueueJob(
				std::to_string(jobResults.size()).c_str(),
				ImGuiControl::Profiler::Color::Blue,
				[&]()
				{
					// 球Vs球
					for (size_t sphereBIndex = sphereIndex + 1; sphereBIndex < _sphereDatas.size(); ++sphereBIndex)
					{
						auto& sphereB = _sphereDatas[sphereBIndex];
						SphereVsSphere(sphereA, sphereB, collisionDataMap);
					}
				}));

			jobResults.emplace_back(JobSystem::Instance().EnqueueJob(
				std::to_string(jobResults.size()).c_str(),
				ImGuiControl::Profiler::Color::Blue,
				[&]()
				{
					// 球Vsボックス
					for (auto& box : _boxDatas)
					{
						SphereVsBox(sphereA, box, collisionDataMap);
					}
				}));

			jobResults.emplace_back(JobSystem::Instance().EnqueueJob(
				std::to_string(jobResults.size()).c_str(),
				ImGuiControl::Profiler::Color::Blue,
				[&]()
				{
					// 球Vsカプセル
					for (auto& capsule : _capsuleDatas)
					{
						SphereVsCapsule(sphereA, capsule, collisionDataMap);
					}
				}));
		}

		// ボックスの当たり判定
		for (size_t boxIndex = 0; boxIndex < _boxDatas.size(); ++boxIndex)
		{
			auto& boxA = _boxDatas[boxIndex];

			jobResults.emplace_back(JobSystem::Instance().EnqueueJob(
				std::to_string(jobResults.size()).c_str(),
				ImGuiControl::Profiler::Color::Blue,
				[&]()
				{
					// ボックスVsボックス
					for (size_t boxBIndex = boxIndex + 1; boxBIndex < _boxDatas.size(); ++boxBIndex)
					{
						auto& boxB = _boxDatas[boxBIndex];
						BoxVsBox(boxA, boxB, collisionDataMap);
					}
				}));

			//      // ボックスVsカプセル
				  //for (auto& capsule : _capsuleColliders)
				  //{
				  //	Transform& transformB = capsule->GetActor()->GetTransform();
				  //	if (Collision3D::IntersectBoxVsCapsule(
			//              boxA->GetPosition().TransformCoord(transformA.GetMatrix()),
			//              boxA->GetHalfSize(),
			//              transformA.GetRotation(),
			//              capsule->GetStart().TransformCoord(transformB.GetMatrix()),
			//              capsule->GetEnd().TransformCoord(transformB.GetMatrix()),
			//              capsule->GetRadius(),
			//              hitPosition,
			//              hitNormal,
			//              penetration))
			//          {
			//              // 接触解消
			//              boxA->Resolve(capsule->GetActor().get(),
			//                  hitPosition,
			//                  hitNormal,
			//                  penetration);
			//              capsule->Resolve(boxA->GetActor().get(),
			//                  hitPosition,
			//                  -hitNormal,
			//                  penetration);
			//          }
				  //}
		}

		// カプセルの当たり判定
		for (size_t capsuleIndex = 0; capsuleIndex < _capsuleDatas.size(); ++capsuleIndex)
		{
			auto& capsuleA = _capsuleDatas[capsuleIndex];

			jobResults.emplace_back(JobSystem::Instance().EnqueueJob(
				std::to_string(jobResults.size()).c_str(),
				ImGuiControl::Profiler::Color::Blue,
				[&]()
				{
					for (size_t capsuleBIndex = capsuleIndex + 1; capsuleBIndex < _capsuleDatas.size(); ++capsuleBIndex)
					{
						auto& capsuleB = _capsuleDatas[capsuleBIndex];
						CapsuleVsCapsule(capsuleA, capsuleB, collisionDataMap);
					}
				}));
		}

		// すべてのジョブの終了を待機
		for (auto& result : jobResults)
		{
			result.get();
		}
	}
	else
	{
		Vector3 hitPosition{};
		Vector3 hitNormal{};
		float penetration = 0.0f;

		// 球の当たり判定
		for (size_t sphereIndex = 0; sphereIndex < _sphereDatas.size(); ++sphereIndex)
		{
			auto& sphereA = _sphereDatas[sphereIndex];

			// 球Vs球
			for (size_t sphereBIndex = sphereIndex + 1; sphereBIndex < _sphereDatas.size(); ++sphereBIndex)
			{
				auto& sphereB = _sphereDatas[sphereBIndex];
				SphereVsSphere(sphereA, sphereB, collisionDataMap);
			}

			// 球Vsボックス
			for (auto& box : _boxDatas)
			{
				SphereVsBox(sphereA, box, collisionDataMap);
			}

			// 球Vsカプセル
			for (auto& capsule : _capsuleDatas)
			{
				SphereVsCapsule(sphereA, capsule, collisionDataMap);
			}
		}

		// ボックスの当たり判定
		for (size_t boxIndex = 0; boxIndex < _boxDatas.size(); ++boxIndex)
		{
			auto& boxA = _boxDatas[boxIndex];

			// ボックスVsボックス
			for (size_t boxBIndex = boxIndex + 1; boxBIndex < _boxDatas.size(); ++boxBIndex)
			{
				auto& boxB = _boxDatas[boxBIndex];
				BoxVsBox(boxA, boxB, collisionDataMap);
			}

			//      // ボックスVsカプセル
				  //for (auto& capsule : _capsuleColliders)
				  //{
				  //	Transform& transformB = capsule->GetActor()->GetTransform();
				  //	if (Collision3D::IntersectBoxVsCapsule(
			//              boxA->GetPosition().TransformCoord(transformA.GetMatrix()),
			//              boxA->GetHalfSize(),
			//              transformA.GetRotation(),
			//              capsule->GetStart().TransformCoord(transformB.GetMatrix()),
			//              capsule->GetEnd().TransformCoord(transformB.GetMatrix()),
			//              capsule->GetRadius(),
			//              hitPosition,
			//              hitNormal,
			//              penetration))
			//          {
			//              // 接触解消
			//              boxA->Resolve(capsule->GetActor().get(),
			//                  hitPosition,
			//                  hitNormal,
			//                  penetration);
			//              capsule->Resolve(boxA->GetActor().get(),
			//                  hitPosition,
			//                  -hitNormal,
			//                  penetration);
			//          }
				  //}
		}

		// カプセルの当たり判定
		for (size_t capsuleIndex = 0; capsuleIndex < _capsuleDatas.size(); ++capsuleIndex)
		{
			auto& capsuleA = _capsuleDatas[capsuleIndex];

			for (size_t capsuleBIndex = capsuleIndex + 1; capsuleBIndex < _capsuleDatas.size(); ++capsuleBIndex)
			{
				auto& capsuleB = _capsuleDatas[capsuleBIndex];
				CapsuleVsCapsule(capsuleA, capsuleB, collisionDataMap);
			}
		}
	}

    // 接触情報を各アクターに通知
	for (auto& [actor, datas] : collisionDataMap)
	{
		if (!actor->IsActive())
			continue;

		std::unordered_map<CollisionLayer, std::vector<Actor*>> currentContactActors;
		// 前フレームに接触したアクターを取得
		auto& lastContactActors = actor->GetLastContactActors();

		for (auto& data : datas)
		{
			// 前フレームに接触していたか確認
			bool isOldContact = std::find(
				lastContactActors[data.myLayer].begin(), lastContactActors[data.myLayer].end(),
				data.other) != lastContactActors[data.myLayer].end();

			// 前フレームで接触していないなら
			if (!isOldContact)
				actor->ContactEnter(data);
			else
				actor->Contact(data);

			currentContactActors[data.myLayer].push_back(data.other);
		}

		// 今回のフレームで接触したアクターの名前を保存
		lastContactActors.clear();
		lastContactActors = currentContactActors;
	}

	// データをクリア
    _sphereDatas.clear();
    _boxDatas.clear();
    _capsuleDatas.clear();
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
// 球データ登録
void CollisionManager::RegisterSphereData(Actor* actor, CollisionLayer layer, CollisionLayerMask mask, const Vector3& position, float radius, bool isTrigger)
{
    _sphereDatas.push_back(SphereData(actor, layer, mask, position, radius, isTrigger));
}
// ボックスデータ登録
void CollisionManager::RegisterBoxData(Actor* actor, CollisionLayer layer, CollisionLayerMask mask, const Vector3& position, const Vector3& halfSize, const Vector3& rotation, bool isTrigger)
{
    _boxDatas.push_back(BoxData(actor, layer, mask, position, halfSize, rotation, isTrigger));
}
// カプセルデータ登録
void CollisionManager::RegisterCapsuleData(Actor* actor, CollisionLayer layer, CollisionLayerMask mask, const Vector3& start, const Vector3& end, float radius, bool isTrigger)
{
    _capsuleDatas.push_back(CapsuleData(actor, layer, mask, start, end, radius, isTrigger));
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

/// 各コンポーネントの情報を設定
void CollisionManager::SetDataByCollider()
{
	for (auto& sphere : _sphereColliders)
	{
		Transform& transform = sphere->GetActor()->GetTransform();
		_sphereDatas.push_back(
			SphereData(
				sphere->GetActor().get(),
                sphere->GetLayer(),
				sphere->GetLayerMask(),
				sphere->GetPosition().TransformCoord(transform.GetMatrix()),
				sphere->GetRadius(),
				sphere->IsTrigger()));
	}
	for (auto& box : _boxColliders)
	{
		Transform& transform = box->GetActor()->GetTransform();
		_boxDatas.push_back(
			BoxData(
				box->GetActor().get(),
                box->GetLayer(),
				box->GetLayerMask(),
				box->GetPosition().TransformCoord(transform.GetMatrix()),
				box->GetHalfSize(),
				transform.GetRotation(),
				box->IsTrigger()));
	}
	for (auto& capsule : _capsuleColliders)
	{
		Transform& transform = capsule->GetActor()->GetTransform();
		_capsuleDatas.push_back(
			CapsuleData(
				capsule->GetActor().get(),
                capsule->GetLayer(),
				capsule->GetLayerMask(),
				capsule->GetStart().TransformCoord(transform.GetMatrix()),
				capsule->GetEnd().TransformCoord(transform.GetMatrix()),
				capsule->GetRadius(),
				capsule->IsTrigger()));
	}
}

// コンテナに情報を追加
void CollisionManager::PushCollisionData(
	std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap,
	Actor* actorA,
	CollisionLayer layerA,
	bool isTriggerA, 
	Actor* actorB, 
	CollisionLayer layerB, 
	bool isTriggerB, 
	const Vector3& hitPosition,
	const Vector3& hitNormal,
	float penetration)
{
	// 接触情報を保存
	CollisionData dataA;
	dataA.myLayer = layerA;
	dataA.isTrigger = isTriggerA;
	dataA.other = actorB;
	dataA.otherIsTrigger = isTriggerB;
	dataA.otherLayer = layerB;
	dataA.hitPosition = hitPosition;
	dataA.hitNormal = hitNormal;
	dataA.penetration = penetration;
	CollisionData dataB;
	dataB.myLayer = layerB;
	dataB.isTrigger = isTriggerB;
	dataB.other = actorA;
	dataB.otherIsTrigger = isTriggerA;
	dataB.otherLayer = layerA;
	dataB.hitPosition = hitPosition;
	dataB.hitNormal = -hitNormal;
	dataB.penetration = penetration;
	{
		// スレッドセーフ
		std::lock_guard<std::mutex> lock(_mutex);
		collisionDataMap[actorA].push_back(dataA);
		collisionDataMap[actorB].push_back(dataB);
	}
}

void CollisionManager::SphereVsSphere(
	const SphereData& sphereA,
	const SphereData& sphereB,
	std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap)
{
	// 同じ場合は処理しない
	if (sphereA.actor == sphereB.actor)return;
	// マスクチェック
	if (!CheckCollisionLayer(
		sphereA.layer, sphereA.mask,
		sphereB.layer, sphereB.mask))
		return;

	Vector3 hitPosition{};
	Vector3 hitNormal{};
	float penetration = 0.0f;
	if (Collision3D::IntersectSphereVsSphere(
		sphereA.position,
		sphereA.radius,
		sphereB.position,
		sphereB.radius,
		hitPosition,
		hitNormal,
		penetration))
	{
		PushCollisionData(
			collisionDataMap,
			sphereA.actor,
			sphereA.layer,
			sphereA.isTrigger,
			sphereB.actor,
			sphereB.layer,
			sphereB.isTrigger,
			hitPosition,
			hitNormal,
			penetration);
	}
}

void CollisionManager::SphereVsBox(
	const SphereData& sphere, 
	const BoxData& box, 
	std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap)
{
	// 同じ場合は処理しない
	if (sphere.actor == box.actor)return;
	// マスクチェック
	if (!CheckCollisionLayer(
		sphere.layer, sphere.mask,
		box.layer, box.mask))
		return;

	Vector3 hitPosition{};
	Vector3 hitNormal{};
	float penetration = 0.0f;
	if (Collision3D::IntersectSphereVsBox(
		sphere.position,
		sphere.radius,
		box.position,
		box.halfSize,
		box.rotation,
		hitPosition,
		hitNormal,
		penetration))
	{
		PushCollisionData(
			collisionDataMap,
			sphere.actor,
			sphere.layer,
			sphere.isTrigger,
			box.actor,
			box.layer,
			box.isTrigger,
			hitPosition,
			hitNormal,
			penetration);
	}
}

void CollisionManager::SphereVsCapsule(
	const SphereData& sphere,
	const CapsuleData& capsule,
	std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap)
{
	// 同じ場合は処理しない
	if (sphere.actor == capsule.actor)return;
	// マスクチェック
	if (!CheckCollisionLayer(
		sphere.layer, sphere.mask,
		capsule.layer, capsule.mask))
		return;

	Vector3 hitPosition{};
	Vector3 hitNormal{};
	float penetration = 0.0f;
	if (Collision3D::IntersectSphereVsCapsule(
		sphere.position,
		sphere.radius,
		capsule.start,
		capsule.end,
		capsule.radius,
		hitPosition,
		hitNormal,
		penetration))
	{
		PushCollisionData(
			collisionDataMap,
			sphere.actor,
			sphere.layer,
			sphere.isTrigger,
			capsule.actor,
			capsule.layer,
			capsule.isTrigger,
			hitPosition,
			hitNormal,
			penetration);
	}
}

void CollisionManager::BoxVsBox(
	const BoxData& boxA, 
	const BoxData& boxB, 
	std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap)
{
	// 同じ場合は処理しない
	if (boxA.actor == boxB.actor)return;
	// マスクチェック
	if (!CheckCollisionLayer(
		boxA.layer, boxA.mask,
		boxB.layer, boxB.mask))
		return;

	Vector3 hitPosition{};
	Vector3 hitNormal{};
	float penetration = 0.0f;
	if (Collision3D::IntersectBoxVsBox(
		boxA.position,
		boxA.halfSize,
		boxA.rotation,
		boxB.position,
		boxB.halfSize,
		boxB.rotation,
		hitPosition,
		hitNormal,
		penetration))
	{
		PushCollisionData(
			collisionDataMap,
			boxA.actor,
			boxA.layer,
			boxA.isTrigger,
			boxB.actor,
			boxB.layer,
			boxB.isTrigger,
			hitPosition,
			hitNormal,
			penetration);
	}
}

void CollisionManager::CapsuleVsCapsule(
	const CapsuleData& capsuleA,
	const CapsuleData& capsuleB,
	std::unordered_map<Actor*, std::vector<CollisionData>>& collisionDataMap)
{
	// 同じ場合は処理しない
	if (capsuleA.actor == capsuleB.actor)return;
	// マスクチェック
	if(!CheckCollisionLayer(
		capsuleA.layer, capsuleA.mask,
		capsuleB.layer, capsuleB.mask))
		return;

	Vector3 hitPosition{};
	Vector3 hitNormal{};
	float penetration = 0.0f;
	if (Collision3D::IntersectCapsuleVsCapsule(
		capsuleA.start,
		capsuleA.end,
		capsuleA.radius,
		capsuleB.start,
		capsuleB.end,
		capsuleB.radius,
		hitPosition,
		hitNormal,
		penetration))
	{
		PushCollisionData(
			collisionDataMap,
			capsuleA.actor,
			capsuleA.layer,
			capsuleA.isTrigger,
			capsuleB.actor,
			capsuleB.layer,
			capsuleB.isTrigger,
			hitPosition,
			hitNormal,
			penetration);
	}
}
