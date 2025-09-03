#include "TerrainActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/Terrain/TerrainController.h"
#include "../../Library/Component/Terrain/TerrainCollider.h"

// ¶¬Žžˆ—
void TerrainActor::OnCreate()
{
    GetTransform().SetScale(50.0f);
    GetTransform().UpdateTransform(nullptr);

    auto terrainController = this->AddComponent<TerrainController>(_terrainPath);
    this->AddCollider<TerrainCollider>();
}
