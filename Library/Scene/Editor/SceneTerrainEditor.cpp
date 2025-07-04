#include "SceneTerrainEditor.h"

#include "../../Library/Component/Terrain/TerrainController.h"
#include "../../Library/Component/Terrain/TerrainDeformer.h"
#include "../../Library/Component/Terrain/TerrainCollider.h"
#include "../../Library/Component/Terrain/TerrainCollisionMaker.h"
#include "../../DebugSupporter/DebugSupporter.h"

void SceneTerrainEditor::OnInitialize()
{
    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // デバッグカメラをオンにする
    Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F4;

    auto terrainActor = RegisterActor<Actor>(u8"Terrain", ActorTag::Stage);
    terrainActor->GetTransform().SetScale(50.0f);
    auto terrainController = terrainActor->AddComponent<TerrainController>();
    terrainActor->AddComponent<TerrainDeformer>();
	terrainActor->AddCollider<TerrainCollider>();
	terrainActor->AddComponent<TerrainCollisionMaker>();
    // パラメータマップの読み込み
    terrainController->LoadParameterMap(L"./Data/Texture/Terrain/ParameterMap.dds");
    // データマップの読み込み
    terrainController->LoadDataMap(L"./Data/Texture/Terrain/TerrainDataMap.dds");
}
