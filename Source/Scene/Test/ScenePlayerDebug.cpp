#include "ScenePlayerDebug.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Actor/Stage/StageManager.h"

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
	static const float CompletionLoadingRate = 1.0f / 8.0f;
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
        RegisterActor<StageManager>("StageManager", ActorTag::Stage);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto player = RegisterActor<PlayerActor>("Player", ActorTag::Player);
        player->GetTransform().SetPosition(Vector3(4.5f, 0.0f, 50.0f));
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto canvas = RegisterActor<InGameCanvasActor>("Canvas", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);
}
