#include "SceneTerrainEditor.h"

#include "../../Library/Component/Terrain/TerrainController.h"
#include "../../Library/Component/Terrain/TerrainDeformer.h"
#include "../../Library/Component/Terrain/TerrainCollider.h"
#include "../../DebugSupporter/DebugSupporter.h"

void SceneTerrainEditor::OnInitialize()
{
    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // デバッグカメラをオンにする
    Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F4;

	auto deformerActor = RegisterActor<Actor>(u8"TerrainDeformer", ActorTag::DrawContextParameter);
    deformerActor->AddComponent<TerrainDeformer>();

    auto terrainActor = RegisterActor<Actor>(u8"Terrain", ActorTag::Stage);
    terrainActor->GetTransform().SetScale(50.0f);
    terrainActor->GetTransform().UpdateTransform(nullptr);
    terrainActor->AddComponent<TerrainController>();
	terrainActor->AddCollider<TerrainCollider>();

    // 境目を合わせるために見る用
    //{
    //    auto testTerrain = RegisterActor<Actor>(u8"TestTerrain0", ActorTag::Stage);
    //    testTerrain->GetTransform().SetPositionZ(100.0f);
    //    testTerrain->GetTransform().SetScale(50.0f);
    //    testTerrain->GetTransform().UpdateTransform(nullptr);
    //    testTerrain->AddComponent<TerrainController>();
    //    testTerrain->AddCollider<TerrainCollider>();
    //}
    //{
    //    auto testTerrain = RegisterActor<Actor>(u8"TestTerrain1", ActorTag::Stage);
    //    testTerrain->GetTransform().SetPositionX(100.0f);
    //    testTerrain->GetTransform().SetScale(50.0f);
    //    testTerrain->GetTransform().UpdateTransform(nullptr);
    //    testTerrain->AddComponent<TerrainController>();
    //    testTerrain->AddCollider<TerrainCollider>();
    //}
    //{
    //    auto testTerrain = RegisterActor<Actor>(u8"TestTerrain2", ActorTag::Stage);
    //    testTerrain->GetTransform().SetPositionX(100.0f);
    //    testTerrain->GetTransform().SetPositionZ(100.0f);
    //    testTerrain->GetTransform().SetScale(50.0f);
    //    testTerrain->GetTransform().UpdateTransform(nullptr);
    //    testTerrain->AddComponent<TerrainController>();
    //    testTerrain->AddCollider<TerrainCollider>();
    //}
}
