#include "SceneTerrainEditor.h"

#include "../../Library/Actor/Terrain/TerrainActor.h"
#include "../../Library/Component/Terrain/TerrainController.h"
#include "../../Library/Component/Terrain/TerrainDeformer.h"
#include "../../Library/Component/Terrain/TerrainCollider.h"

void SceneTerrainEditor::OnInitialize()
{
    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // デバッグカメラをオンにする
    Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F4;

	auto deformerActor = RegisterActor<Actor>(u8"TerrainDeformer", ActorTag::System);
    deformerActor->AddComponent<TerrainDeformer>();

    auto stage0 = RegisterActor<TerrainActor>("Stage0", 
        ActorTag::Stage, 
        "./Data/Terrain/Save/001.json",
        Vector3(0.0f, 0.0f, 0.0f));
    auto stage1 = RegisterActor<TerrainActor>("Stage1", 
        ActorTag::Stage, 
        "./Data/Terrain/Save/002.json",
        Vector3(100.0f, 0.0f, 0.0f));
    //auto stage2 = RegisterActor<TerrainActor>("Stage2", 
    //    ActorTag::Stage, 
    //    "./Data/Terrain/Save/Test000.json",
    //    Vector3(0.0f, 0.0f, 100.0f));
    //auto stage3 = RegisterActor<TerrainActor>("Stage3", 
    //    ActorTag::Stage, 
    //    "./Data/Terrain/Save/Test000.json",
    //    Vector3(100.0f, 0.0f, 100.0f));
}
