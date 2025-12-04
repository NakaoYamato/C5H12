#include "ScenePlayerDebug.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Actor/Terrain/TerrainActor.h"

// コンポーネント
#include "../../Source/AI/MetaAI.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Enemy/Dummy/DummyActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"
#include "../../Source/Stage/Props/Chest/ChestActor.h"
#include "../../Source/Stage/Props/Fence/FenceActor.h"

#include "../../InGame/InputManager.h"
#include "../../InGame/InGameCanvasActor.h"

void ScenePlayerDebug::OnInitialize()
{
	static const float CompletionLoadingRate = 1.0f / 11.0f;
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");
	AddCompletionLoading(CompletionLoadingRate);

    // オブジェクト作成
    {
        auto metaAI = RegisterActor<Actor>("MetaAI", ActorTag::System);
		metaAI->AddComponent<MetaAI>();
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto inputManager = RegisterActor<InputManager>("InputManager", ActorTag::System);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto stage0 = RegisterActor<TerrainActor>("Stage0", ActorTag::Stage, "./Data/Terrain/Save/001.json", Vector3(0.0f, 0.0f, 0.0f));
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto stage1 = RegisterActor<TerrainActor>("Stage1", ActorTag::Stage, "./Data/Terrain/Save/002.json", Vector3(100.0f, 0.0f, 0.0f));
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto Chest = RegisterActor<ChestActor>("Chest", ActorTag::Stage);
        Chest->GetTransform().SetPosition(Vector3(0.0f, 0.0f, 5.0f));
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto Fence = RegisterActor<FenceActor>("Fence", ActorTag::Stage);
        Fence->GetTransform().SetPosition(Vector3(-43.5f, 0.0f, 0.45f));
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto player = RegisterActor<PlayerActor>("Player", ActorTag::Player);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto canvas = RegisterActor<InGameCanvasActor>("Canvas", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);
}
