#include "ModelCollider.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 開始処理
void ModelCollider::Start()
{
	// モデルの当たり判定情報を読み込む
	_modelCollision.Load(GetActor()->GetModel());
}

// 更新処理
void ModelCollider::Update(float elapsedTime)
{
	CollisionManager& collisionManager = GetActor()->GetScene()->GetCollisionManager();
	auto& poseNodes = GetActor()->GetModel().lock()->GetPoseNodes();
	auto& sphereDatas = _modelCollision.GetSphereDatas();
	auto& capsuleDatas = _modelCollision.GetCapsuleDatas();

	// モデルの当たり判定情報をCollisionManagerに登録する
	for (auto& sphere : sphereDatas)
	{
		if (sphere.nodeIndex == -1)
			continue;
		auto& node = poseNodes[sphere.nodeIndex];
		collisionManager.RegisterSphereData(
			GetActor().get(),
			"Collider",
			sphere.position.TransformCoord(node.worldTransform),
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
		collisionManager.RegisterCapsuleData(
			GetActor().get(),
			"Collider",
			capsule.start.TransformCoord(startNode.worldTransform),
			capsule.end.TransformCoord(endNode.worldTransform),
			capsule.radius,
			false);
	}
}

// デバッグ描画処理
void ModelCollider::DebugRender(const RenderContext& rc)
{
	_modelCollision.DebugRender();
}

// GUI描画
void ModelCollider::DrawGui()
{
	_modelCollision.DrawGui(false);
}
