#include "SceneTerrainEditor.h"

#include "../../Library/Component/TerrainController.h"

void SceneTerrainEditor::OnInitialize()
{
    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    auto terrainActor = RegisterActor<Actor>(u8"Terrain", ActorTag::Stage);
    terrainActor->AddComponent<TerrainController>();
}
