#include "SceneWyvernDebug.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Actor/Stage/StageManager.h"

// コンポーネント
#include "../../Source/AI/MetaAI.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"

#include "../../InGame/InputManager.h"
#include "../../InGame/InGameCanvasActor.h"

void SceneWyvernDebug::OnInitialize()
{
    static const float CompletionLoadingRate = 1.0f / 7.0f;
    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");
    AddCompletionLoading(CompletionLoadingRate);

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
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto canvas = RegisterActor<InGameCanvasActor>("Canvas", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto wyvern = RegisterActor<WyvernActor>("Wyvern", ActorTag::Enemy);
        wyvern->GetTransform().SetPosition(Vector3(0.0f, 5.0f, 20.0f));
    }
    AddCompletionLoading(CompletionLoadingRate);
}
