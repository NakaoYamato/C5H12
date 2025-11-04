#include "TerrainActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/Terrain/TerrainController.h"
#include "../../Library/Component/Terrain/TerrainCollider.h"

#include "../../Library/Component/StageEffectEmitter.h"

// ¶¬Žžˆ—
void TerrainActor::OnCreate()
{
    GetTransform().SetScale(50.0f);
    GetTransform().UpdateTransform(nullptr);

    this->AddComponent<TerrainController>(_terrainPath);
	auto effect = this->AddComponent<EffectController>();
	this->AddComponent<StageEffectEmitter>();

	effect->LoadEffekseerEffect(0, "./Data/Effect/Effekseer/Player/Attack_Impact.efk");

    auto collider = this->AddCollider<TerrainCollider>();

	collider->SetLayer(CollisionLayer::Stage);
}
