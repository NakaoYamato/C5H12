#include "TerrainEnvironmentController.h"

#include "../../Library/Component/Collider/BoxCollider.h"
#include "../../Library/Component/Collider/SphereCollider.h"
#include "../../Library/Component/Collider/CapsuleCollider.h"
#include "../../Library/Component/Collider/MeshCollider.h"

#include "../InstancingModelRenderer.h"

#include <imgui.h>

// 生成時処理
void TerrainEnvironmentController::OnCreate()
{
	auto objectLayout = _terrain.lock()->GetTerrainObjectLayout();
	auto& myLayout = objectLayout->GetLayouts().at(_layoutID);

	const std::string& modelPath = myLayout.modelPath;
	
	GetActor()->AddComponent<InstancingModelRenderer>(objectLayout->GetModel(modelPath).lock());

	// トランスフォーム更新
	GetActor()->GetTransform().SetPosition(myLayout.localPosition.TransformCoord(_terrainTransform));
	GetActor()->GetTransform().SetRotation(myLayout.rotation);
	GetActor()->GetTransform().SetScale(myLayout.size);
	GetActor()->GetTransform().UpdateTransform(nullptr);

	// 衝突属性からコライダーを追加
	switch (myLayout.collisionType)
	{
	case TerrainObjectLayout::CollisionType::Box:
	case TerrainObjectLayout::CollisionType::BoxTrigger:
	{
		auto collider = GetActor()->AddCollider<BoxCollider>();
		collider->SetPosition(myLayout.collisionOffset);
		collider->SetRadius(myLayout.collisionParameter.Vec3());
		collider->SetTrigger(myLayout.collisionType == TerrainObjectLayout::CollisionType::BoxTrigger); // トリガーの場合はトリガー設定
		collider->SetLayer(CollisionLayer::Stage);
	}
		break;
	case TerrainObjectLayout::CollisionType::Sphere:
	case TerrainObjectLayout::CollisionType::SphereTrigger:
	{
		auto collider = GetActor()->AddCollider<SphereCollider>();
		collider->SetPosition(myLayout.collisionOffset);
		collider->SetRadius(myLayout.collisionParameter.x); // 球の半径はX成分に格納
		collider->SetTrigger(myLayout.collisionType == TerrainObjectLayout::CollisionType::SphereTrigger); // トリガーの場合はトリガー設定
		collider->SetLayer(CollisionLayer::Stage);
	}
		break;
	case TerrainObjectLayout::CollisionType::Capsule:
	case TerrainObjectLayout::CollisionType::CapsuleTrigger:
	{
		auto collider = GetActor()->AddCollider<CapsuleCollider>();
		collider->SetStart(myLayout.collisionOffset);
		collider->SetEnd(myLayout.collisionParameter.Vec3());
		collider->SetRadius(myLayout.collisionParameter.w);
		collider->SetTrigger(myLayout.collisionType == TerrainObjectLayout::CollisionType::CapsuleTrigger); // トリガーの場合はトリガー設定
		collider->SetLayer(CollisionLayer::Stage);
	}
		break;
	case TerrainObjectLayout::CollisionType::Mesh:
	case TerrainObjectLayout::CollisionType::MeshTrigger:
	{
		// TODO
		//auto collider = GetActor()->AddCollider<MeshCollider>();
		//collider->SetModelPath(modelPath);
		//collider->SetCollisionOffset(myLayout.collisionOffset);
		//collider->SetCollisionParameter(myLayout.collisionParameter);
		//collider->SetTrigger(myLayout.collisionType == TerrainObjectLayout::CollisionType::MeshTrigger); // トリガーの場合はトリガー設定
	}
		break;
	}
}
// 更新処理
void TerrainEnvironmentController::Update(float elapsedTime)
{
	// 地形が取得できていない場合は自身を削除
	if (_terrain.lock() == nullptr)
	{
		GetActor()->Remove();
		return;
	}
	// 自身のIDが地形の配置情報に存在しないなら自身を削除
	auto myLayout = _terrain.lock()->GetTerrainObjectLayout()->FindLayout(_layoutID);
	if (myLayout == nullptr)
	{
		GetActor()->Remove();
		return;
	}
	// 地形の配置情報に参照されているモデルが存在しないなら自身を削除
	if (!_terrain.lock()->GetTerrainObjectLayout()->HasModel(myLayout->modelPath))
	{
		GetActor()->Remove();
		return;
	}
	// 地形の配置情報からトランスフォーム情報更新
	if (_overwrite)
	{
		GetActor()->GetTransform().SetPosition(myLayout->localPosition.TransformCoord(_terrainTransform));
		GetActor()->GetTransform().SetRotation(myLayout->rotation);
		GetActor()->GetTransform().SetScale(myLayout->size);
		GetActor()->GetTransform().UpdateTransform(nullptr);
	}
}
// GUI描画
void TerrainEnvironmentController::DrawGui()
{
	ImGui::Text(u8"配置番号: %d", _layoutID);
	ImGui::Checkbox(u8"地形の配置情報で上書きするか", &_overwrite);
}
