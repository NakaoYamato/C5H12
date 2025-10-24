#include "SceneDecalEditor.h"

#include "../../Library/Component/DecalController.h"
#include "../../Library/Component/Terrain/TerrainController.h"
#include "../../Library/Component/Terrain/TerrainDeformer.h"
#include "../../Library/Component/Terrain/TerrainCollider.h"

// 初期化
void SceneDecalEditor::OnInitialize()
{
    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

	auto actor = RegisterActor<Actor>(u8"Decal", ActorTag::Stage);
	actor->AddComponent<DecalController>(
		L"./Data/Texture/Decal/gun_holes.png",
		L"./Data/Texture/Decal/NormalMap.png");
	actor->GetTransform().SetAngleX(DirectX::XM_PIDIV2);
	actor->GetTransform().SetScale(10.0f);

	// デバッグカメラをオンにする
	Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F4;
	// グリッド表示
	SetShowGrid(true);

	auto terrainActor = RegisterActor<Actor>(u8"Terrain", ActorTag::Stage);
	terrainActor->GetTransform().SetScale(50.0f);
	terrainActor->GetTransform().UpdateTransform(nullptr);
	terrainActor->AddComponent<TerrainController>();
	terrainActor->AddCollider<TerrainCollider>();
}
