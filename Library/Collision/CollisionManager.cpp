#include "CollisionManager.h"

#include <imgui.h>

#include "CollisionMath.h"
#include "../../Library/JobSystem/JobSystem.h"
#include "../DebugSupporter/DebugSupporter.h"

void CollisionManager::Update()
{
	// 衝突情報のコンテナ
    std::unordered_map<Actor*, std::vector<CollisionData>> collisionDataMap;
	// コンテナに情報を追加
	auto PushCollisionData = [&](
		Actor* actorA, 
		const std::string& layerA,
		Actor* actorB,
		const std::string& layerB,
		const Vector3& hitPosition,
		const Vector3& hitNormal,
		float penetration)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);

			// 接触情報を保存
			CollisionData dataA;
            dataA.myLayer = layerA;
			dataA.other = actorB;
            dataA.otherLayer = layerB;
			dataA.hitPosition = hitPosition;
			dataA.hitNormal = hitNormal;
			dataA.penetration = penetration;
			collisionDataMap[actorA].push_back(dataA);
			CollisionData dataB;
			dataB.other = actorA;
			dataB.hitPosition = hitPosition;
			dataB.hitNormal = -hitNormal;
			dataB.penetration = penetration;
			collisionDataMap[actorB].push_back(dataB);
		};

	// 各コンポーネントの情報を設定
	SetDataByCollider();

	// 全体の計算時間
	ProfileScopedSection_2(0, ActorManager, ImGuiControl::Profiler::Purple);
    // ジョブシステムでマルチスレッドを使用する場合
	if (JobSystem::Instance().UseMultiThread())
	{
		std::vector<std::future<void>> jobResults;

		// 球の当たり判定
		for (auto& sphereA : _sphereDatas)
		{
			// 球Vs球
			for (auto& sphereB : _sphereDatas)
			{
				// 同じ場合は処理しない
				if (sphereA.actor == sphereB.actor)
					continue;

				jobResults.emplace_back(JobSystem::Instance().EnqueueJob(
					std::to_string(jobResults.size()).c_str(),
					ImGuiControl::Profiler::Color::Blue,
					[&]()
					{
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
								sphereA.actor,
								sphereA.layer,
								sphereB.actor,
								sphereB.layer,
								hitPosition,
								hitNormal,
								penetration);
						}
					}
				));
			}

			// 球Vsボックス
			for (auto& box : _boxDatas)
			{
				jobResults.emplace_back(JobSystem::Instance().EnqueueJob(
					std::to_string(jobResults.size()).c_str(),
					ImGuiControl::Profiler::Color::Blue,
					[&]()
					{
						Vector3 hitPosition{};
						Vector3 hitNormal{};
						float penetration = 0.0f;

						if (Collision3D::IntersectSphereVsBox(
							sphereA.position,
							sphereA.radius,
							box.position,
							box.halfSize,
							box.rotation,
							hitPosition,
							hitNormal,
							penetration))
						{
							PushCollisionData(
								sphereA.actor,
								sphereA.layer,
								box.actor,
								box.layer,
								hitPosition,
								hitNormal,
								penetration);
						}
					}
				));
			}

			// 球Vsカプセル
			for (auto& capsule : _capsuleDatas)
			{
				jobResults.emplace_back(JobSystem::Instance().EnqueueJob(
					std::to_string(jobResults.size()).c_str(),
					ImGuiControl::Profiler::Color::Blue,
					[&]()
					{
						Vector3 hitPosition{};
						Vector3 hitNormal{};
						float penetration = 0.0f;

						if (Collision3D::IntersectSphereVsCapsule(
							sphereA.position,
							sphereA.radius,
							capsule.start,
							capsule.end,
							capsule.radius,
							hitPosition,
							hitNormal,
							penetration))
						{
							PushCollisionData(
								sphereA.actor,
								sphereA.layer,
								capsule.actor,
								capsule.layer,
								hitPosition,
								hitNormal,
								penetration);
						}
					}
				));
			}
		}

		// ボックスの当たり判定
		for (auto& boxA : _boxDatas)
		{
			// ボックスVsボックス
			for (auto& boxB : _boxDatas)
			{
				// 同じ場合は処理しない
				if (boxA.actor == boxB.actor)
					continue;

				jobResults.emplace_back(JobSystem::Instance().EnqueueJob(
					std::to_string(jobResults.size()).c_str(),
					ImGuiControl::Profiler::Color::Blue,
					[&]()
					{
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
								boxA.actor,
								boxA.layer,
								boxB.actor,
								boxB.layer,
								hitPosition,
								hitNormal,
								penetration);
						}
					}
				));
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
		for (auto& capsuleA : _capsuleDatas)
		{
			for (auto& capsuleB : _capsuleDatas)
			{
				// 同じ場合は処理しない
				if (capsuleA.actor == capsuleB.actor)
					continue;

				jobResults.emplace_back(JobSystem::Instance().EnqueueJob(
					std::to_string(jobResults.size()).c_str(),
					ImGuiControl::Profiler::Color::Blue,
					[&]()
					{
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
								capsuleA.actor,
								capsuleA.layer,
								capsuleB.actor,
								capsuleB.layer,
								hitPosition,
								hitNormal,
								penetration);
						}
					}
				));
			}
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
		for (auto& sphereA : _sphereDatas)
		{
			// 球Vs球
			for (auto& sphereB : _sphereDatas)
			{
				// 同じ場合は処理しない
				if (sphereA.actor == sphereB.actor)
					continue;

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
						sphereA.actor,
                        sphereA.layer,
						sphereB.actor,
                        sphereB.layer,
						hitPosition,
						hitNormal,
						penetration);
				}
			}

			// 球Vsボックス
			for (auto& box : _boxDatas)
			{
				if (Collision3D::IntersectSphereVsBox(
					sphereA.position,
					sphereA.radius,
					box.position,
					box.halfSize,
					box.rotation,
					hitPosition,
					hitNormal,
					penetration))
				{
					PushCollisionData(
						sphereA.actor,
                        sphereA.layer,
                        box.actor,
                        box.layer,
						hitPosition,
						hitNormal,
						penetration);
				}
			}

			// 球Vsカプセル
			for (auto& capsule : _capsuleDatas)
			{
				if (Collision3D::IntersectSphereVsCapsule(
					sphereA.position,
					sphereA.radius,
					capsule.start,
					capsule.end,
					capsule.radius,
					hitPosition,
					hitNormal,
					penetration))
				{
					PushCollisionData(
                        sphereA.actor,
                        sphereA.layer,
                        capsule.actor,
                        capsule.layer,
						hitPosition,
						hitNormal,
						penetration);
				}
			}
		}

		// ボックスの当たり判定
		for (auto& boxA : _boxDatas)
		{
			// ボックスVsボックス
			for (auto& boxB : _boxDatas)
			{
				// 同じ場合は処理しない
				if (boxA.actor == boxB.actor)
					continue;

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
                        boxA.actor,
                        boxA.layer,
                        boxB.actor,
                        boxB.layer,
						hitPosition,
						hitNormal,
						penetration);
				}
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
		for (auto& capsuleA : _capsuleDatas)
		{
			for (auto& capsuleB : _capsuleDatas)
			{
				// 同じ場合は処理しない
				if (capsuleA.actor == capsuleB.actor)
					continue;

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
                        capsuleA.actor,
                        capsuleA.layer,
                        capsuleB.actor,
                        capsuleB.layer,
						hitPosition,
						hitNormal,
						penetration);
				}
			}
		}
	}

    // 接触情報を各アクターに通知
	for (auto& [actor, datas] : collisionDataMap)
	{
		for (auto& data : datas)
		{
			actor->Contact(data);
		}
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
void CollisionManager::RegisterSphereData(Actor* actor, std::string layer, const Vector3& position, float radius)
{
    _sphereDatas.push_back(SphereData(actor, layer, position, radius));
}
// ボックスデータ登録
void CollisionManager::RegisterBoxData(Actor* actor, std::string layer, const Vector3& position, const Vector3& halfSize, const Vector3& rotation)
{
    _boxDatas.push_back(BoxData(actor, layer, position, halfSize, rotation));
}
// カプセルデータ登録
void CollisionManager::RegisterCapsuleData(Actor* actor, std::string layer, const Vector3& start, const Vector3& end, float radius)
{
    _capsuleDatas.push_back(CapsuleData(actor, layer, start, end, radius));
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
				sphere->GetPosition().TransformCoord(transform.GetMatrix()),
				sphere->GetRadius()));
	}
	for (auto& box : _boxColliders)
	{
		Transform& transform = box->GetActor()->GetTransform();
		_boxDatas.push_back(
			BoxData(
				box->GetActor().get(),
                box->GetLayer(),
				box->GetPosition().TransformCoord(transform.GetMatrix()),
				box->GetHalfSize(),
				transform.GetRotation()));
	}
	for (auto& capsule : _capsuleColliders)
	{
		Transform& transform = capsule->GetActor()->GetTransform();
		_capsuleDatas.push_back(
			CapsuleData(
				capsule->GetActor().get(),
                capsule->GetLayer(),
				capsule->GetStart().TransformCoord(transform.GetMatrix()),
				capsule->GetEnd().TransformCoord(transform.GetMatrix()),
				capsule->GetRadius()));
	}
}
