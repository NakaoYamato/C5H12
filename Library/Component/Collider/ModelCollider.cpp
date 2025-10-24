#include "ModelCollider.h"

#include "../../Library/Collision/CollisionMath.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 生成時処理
void ModelCollider::OnCreate()
{
	// モデルの当たり判定情報を読み込む
	_modelCollision.Load(GetActor()->GetModel());

	// 当たり判定情報のタグ名から子供オブジェクトを生成
	std::string actorName = GetActor()->GetName();
	for (auto& tag : _modelCollision.GetTags())
	{
		auto tagActor = GetActor()->GetScene()->RegisterActor<Actor>(actorName + tag, GetActor()->GetTag());
		tagActor->SetParent(GetActor().get());
		_tagActors[tag] = tagActor;
	}
}

// 開始処理
void ModelCollider::Start()
{
	// アニメータを取得
	_animator = GetActor()->GetComponent<Animator>();
}
// 更新処理
void ModelCollider::Update(float elapsedTime)
{ 
	// 攻撃判定が出たかどうかを初期化
	_collAttackEvent = false;

	CollisionManager& collisionManager = GetActor()->GetScene()->GetCollisionManager();
	auto& poseNodes = GetActor()->GetModel().lock()->GetPoseNodes();
	auto& sphereDatas = _modelCollision.GetSphereDatas();
	auto& capsuleDatas = _modelCollision.GetCapsuleDatas();
	auto& tagNames = _modelCollision.GetTags();

	std::vector<CollisionManager::SphereData> animationEventSphereDatas;
	std::vector<CollisionManager::CapsuleData> animationEventCapsuleDatas;
	// アニメーションイベントの当たり判定更新
	UpdateAnimationEventCollision(elapsedTime, animationEventSphereDatas, animationEventCapsuleDatas);

	// モデルの当たり判定情報をCollisionManagerに登録する
	for (auto& sphere : sphereDatas)
	{
		if (sphere.nodeIndex == -1)
			continue;
		auto& node = poseNodes[sphere.nodeIndex];
		Actor* actor = sphere.tagIndex != -1 ? 
			_tagActors[tagNames.at(sphere.tagIndex)].get() :
			GetActor().get();
		Vector3 worldPosition = sphere.position.TransformCoord(node.worldTransform);

		// アニメーションイベントの当たり判定と接触する場合当たり判定を登録しない
		if (IntersectAnimationEvent(
			worldPosition,
			sphere.radius,
			animationEventSphereDatas,
			animationEventCapsuleDatas))
			continue;

		collisionManager.RegisterSphereData(
			actor,
			GetLayer(),
			GetLayerMask(),
			worldPosition,
			sphere.radius,
			false);
	}
	for (auto& capsule : capsuleDatas)
	{
		if (capsule.startNodeIndex == -1)
			continue;
		if (capsule.endNodeIndex == -1)
			continue;
		auto& startNode = poseNodes[capsule.startNodeIndex];
		auto& endNode = poseNodes[capsule.endNodeIndex];
		Actor* actor = capsule.tagIndex != -1 ?
			_tagActors[tagNames.at(capsule.tagIndex)].get() :
			GetActor().get();
		Vector3 worldStart = capsule.start.TransformCoord(startNode.worldTransform);
		Vector3 worldEnd = capsule.end.TransformCoord(endNode.worldTransform);

		// アニメーションイベントの当たり判定と接触する場合当たり判定を登録しない
		if (IntersectAnimationEvent(
			worldStart,
			worldEnd,
			capsule.radius,
			animationEventSphereDatas, 
			animationEventCapsuleDatas))
			continue;

		collisionManager.RegisterCapsuleData(
			actor,
			GetLayer(),
			GetLayerMask(),
			worldStart,
			worldEnd,
			capsule.radius,
			false);
	}
}
// デバッグ描画処理
void ModelCollider::DebugRender(const RenderContext& rc)
{
	_modelCollision.DebugRender(rc);
}
// GUI描画
void ModelCollider::DrawGui()
{
	ImGui::Checkbox(u8"攻撃判定イベント", &_collAttackEvent);
	_modelCollision.DrawGui(false);
}
// アニメーションイベントの当たり判定更新
void ModelCollider::UpdateAnimationEventCollision(float elapsedTime,
	std::vector<CollisionManager::SphereData>& sphereDatas,
	std::vector<CollisionManager::CapsuleData>& capsuleDatas)
{
	// アニメータが存在していなければ処理しない
	if (!_animator.lock())
		return;

	auto& poseNodes = GetActor()->GetModel().lock()->GetPoseNodes();
	
	// 現在のアニメーションイベントを取得し、それにふくまれているイベントを処理する
	auto events = _animator.lock()->GetCurrentEvents();
	for (auto& event : events)
	{
		// イベントの種類がフラグであれば処理しない
		if (event.eventType == AnimationEvent::EventType::Flag)
			continue;

		auto& collisionManager = GetActor()->GetScene()->GetCollisionManager();

		auto& node = poseNodes[event.nodeIndex];
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&event.position));
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(event.angle.x, event.angle.y, event.angle.z);
		DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&event.scale));
		DirectX::XMFLOAT4X4 transform = {};

		DirectX::XMStoreFloat4x4(&transform, S * R * T * DirectX::XMLoadFloat4x4(&node.worldTransform));

		CollisionLayer layer = CollisionLayer::None;
		CollisionLayerMask layerMask = CollisionLayerMaskAll;
		if (event.eventType == AnimationEvent::EventType::Attack)
		{
			layer = CollisionLayer::Attack;
			// 被弾レイヤー以外に当たらないようにする
			layerMask = GetCollisionLayerMask(CollisionLayer::Hit);
			// 攻撃判定が出た
			_collAttackEvent = true;
		}
		else if (event.eventType == AnimationEvent::EventType::Hit)
		{
			layer = CollisionLayer::Hit;
		}

		// 判定生成
		switch (event.shapeType)
		{
		case AnimationEvent::ShapeType::Box:
			// TODO: Boxの当たり判定は未実装
			//collisionManager.RegisterBoxData(
			//	GetActor().get(),
			//	layer,
			//	layerMask,
			//	event.position.TransformCoord(node.worldTransform),
			//	event.scale,
			//	event.angle);
			break;
		case AnimationEvent::ShapeType::Sphere:
			collisionManager.RegisterSphereData(
				GetActor().get(),
				layer,
				layerMask,
				Vector3(transform._41, transform._42, transform._43),
				event.scale.x);
			// アニメーションイベントの球体当たり判定データを追加
			sphereDatas.emplace_back(
				GetActor().get(),
				layer,
				layerMask,
				Vector3(transform._41, transform._42, transform._43),
				event.scale.x,
				false);
			break;
		case AnimationEvent::ShapeType::Capsule:
			collisionManager.RegisterCapsuleData(
				GetActor().get(),
				layer,
				layerMask,
				event.position.TransformCoord(node.worldTransform),
				event.angle.TransformCoord(node.worldTransform),
				event.scale.x);
			// アニメーションイベントのカプセル当たり判定データを追加
			capsuleDatas.emplace_back(
				GetActor().get(),
				layer,
				layerMask,
				event.position.TransformCoord(node.worldTransform),
				event.angle.TransformCoord(node.worldTransform),
				event.scale.x,
				false);
			break;
		}
	}
}
// モデルの球体当たり判定がアニメーションイベントの当たり判定と接触しているか確認
bool ModelCollider::IntersectAnimationEvent(
	const Vector3& modelSpherePosition,
	float modelSphereRadius,
	const std::vector<CollisionManager::SphereData>& eventSphereDatas,
	const std::vector<CollisionManager::CapsuleData>& eventCapsuleDatas) const
{
	Vector3 hitPosition{}, hitNormal{};
	float penetration = 0.0f;

	// モデルの球体当たり判定とアニメーションイベントの球体当たり判定が接触しているか確認
	for (const auto& eventSphere : eventSphereDatas)
	{
		if (Collision3D::IntersectSphereVsSphere(
			modelSpherePosition,
			modelSphereRadius,
			eventSphere.position,
			eventSphere.radius,
			hitPosition,
			hitNormal,
			penetration))
		{
			return true;
		}
	}
	// モデルの球体当たり判定とアニメーションイベントのカプセル当たり判定が接触しているか確認
	for (const auto& eventCapsule : eventCapsuleDatas)
	{
		if (Collision3D::IntersectSphereVsCapsule(
			modelSpherePosition,
			modelSphereRadius,
			eventCapsule.start,
			eventCapsule.end,
			eventCapsule.radius,
			hitPosition,
			hitNormal,
			penetration))
		{
			return true;
		}
	}

	return false;
}
// モデルのカプセル当たり判定がアニメーションイベントの当たり判定と接触しているか確認
bool ModelCollider::IntersectAnimationEvent(
	const Vector3& modelCapsuleStart,
	const Vector3& modelCapsuleEnd,
	float modelCapsuleRadius,
	const std::vector<CollisionManager::SphereData>& eventSphereDatas,
	const std::vector<CollisionManager::CapsuleData>& eventCapsuleDatas) const
{
	Vector3 hitPosition{}, hitNormal{};
	float penetration = 0.0f;
	// モデルのカプセル当たり判定とアニメーションイベントの球体当たり判定が接触しているか確認
	for (const auto& eventSphere : eventSphereDatas)
	{
		if (Collision3D::IntersectSphereVsCapsule(
			eventSphere.position,
			eventSphere.radius,
			modelCapsuleStart,
			modelCapsuleEnd,
			modelCapsuleRadius,
			hitPosition,
			hitNormal,
			penetration))
		{
			return true;
		}
	}
	// モデルのカプセル当たり判定とアニメーションイベントのカプセル当たり判定が接触しているか確認
	for (const auto& eventCapsule : eventCapsuleDatas)
	{
		if (Collision3D::IntersectCapsuleVsCapsule(
			modelCapsuleStart,
			modelCapsuleEnd,
			modelCapsuleRadius,
			eventCapsule.start,
			eventCapsule.end,
			eventCapsule.radius,
			hitPosition,
			hitNormal,
			penetration))
		{
			return true;
		}
	}
	return false;
}
