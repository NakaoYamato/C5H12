#include "SceneWyvernDebug.h"

#include "../../Library/Graphics/Graphics.h"

// コンポーネント
#include "../../Source/AI/MetaAI.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Stage/StageActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"

#include "../../InGame/InputMediator.h"
#include "../../InGame/CanvasMediator.h"
#include "../../Menu/MenuMediator.h"

void SceneWyvernDebug::OnInitialize()
{
    static const float CompletionLoadingRate = 1.0f / 8.0f;
    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");
    AddCompletionLoading(CompletionLoadingRate);

    {
        auto metaAI = RegisterActor<Actor>("MetaAI", ActorTag::DrawContextParameter);
        metaAI->AddComponent<MetaAI>();
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto inputMediator = RegisterActor<InputMediator>("InputMediator", ActorTag::DrawContextParameter);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto stage = RegisterActor<StageActor>("Stage", ActorTag::Stage);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto player = RegisterActor<PlayerActor>("Player", ActorTag::Player);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto menu = RegisterActor<MenuMediator>("MenuMediator", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto canvas = RegisterActor<CanvasMediator>("CanvasMediator", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto wyvern = RegisterActor<WyvernActor>("Wyvern", ActorTag::Enemy);
        wyvern->GetTransform().SetPosition(Vector3(0.0f, 5.0f, 20.0f));
    }
    AddCompletionLoading(CompletionLoadingRate);
}
