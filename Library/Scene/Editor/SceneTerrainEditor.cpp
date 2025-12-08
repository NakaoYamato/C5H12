#include "SceneTerrainEditor.h"

#include "../../Library/Actor/Stage/StageManager.h"
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


	RegisterActor<StageManager>("StageManager", ActorTag::Stage);
}
