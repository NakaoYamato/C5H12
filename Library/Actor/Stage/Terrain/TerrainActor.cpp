#include "TerrainActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/Terrain/TerrainController.h"
#include "../../Library/Component/Terrain/TerrainCollider.h"

#include "../../Library/Component/StageEffectEmitter.h"

// ¶¬Žžˆ—
void TerrainActor::OnCreate()
{
    GetTransform().UpdateTransform(nullptr);

    this->AddComponent<TerrainController>(_terrainPath);
	auto effect = this->AddComponent<EffectController>();
	this->AddComponent<StageEffectEmitter>();

	auto dustEffect = effect->LoadEffekseerEffect(0, "./Data/Effect/Effekseer/Stage/DustCloud.efk");
	dustEffect->SetScale(Vector3(0.25f, 0.5f, 0.25f));
	dustEffect->SetAllColor(Vector4(139.0f / 255.0f, 112.0f / 255.0f, 91.0f / 255.0f, 1.0f));

    auto collider = this->AddCollider<TerrainCollider>();

	collider->SetLayer(CollisionLayer::Stage);
}
