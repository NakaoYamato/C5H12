#include "StageManager.h"

#include "../../Library/Scene/Scene.h"

#include "../../Library/Component/Collider/TransparentWallCollider.h"
#include "../../Library/Component/EnvironmentController.h"

#include "../../Library/Component/Terrain/TerrainDeformer.h"
#include "Terrain/TerrainActor.h"

// ¶¬Žžˆ—
void StageManager::OnCreate()
{
	// Ž©g‚ð“®‚©‚¹‚È‚¢‚æ‚¤‚É‚·‚é
	this->SetIsDrawingTransformGui(false);

	// TerrainDeformer‚ð’Ç‰Á
	this->AddComponent<TerrainDeformer>();
	this->AddComponent<EnvironmentController>();
	auto collider = this->AddCollider<TransparentWallCollider>();
	collider->SetLayer(CollisionLayer::Stage);

	// Žq‹Ÿ’Ç‰Á
	auto stage0 = this->GetScene()->RegisterActor<TerrainActor>("Stage0",
		ActorTag::Stage,
		"./Data/Terrain/Save/001.json",
		Vector3(0.0f, 0.0f, 0.0f));
	this->AddChild(stage0);
	auto stage1 = this->GetScene()->RegisterActor<TerrainActor>("Stage1",
		ActorTag::Stage,
		"./Data/Terrain/Save/002.json",
		Vector3(100.0f, 0.0f, 0.0f));
	this->AddChild(stage1);
}
