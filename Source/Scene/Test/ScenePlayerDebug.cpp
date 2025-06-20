#include "ScenePlayerDebug.h"

#include "../../Library/Graphics/Graphics.h"

// コンポーネント
#include "../../Source/AI/MetaAI.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Stage/StageActor.h"
#include "../../Source/Enemy/Dummy/DummyActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"

#include "../../Mediator/InputMediator.h"
#include "../../Mediator/CanvasMediator.h"
#include "../../Mediator/MenuMediator.h"

void ScenePlayerDebug::OnInitialize()
{
	static const float CompletionLoadingRate = 0.1f;
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");
	AddCompletionLoading(CompletionLoadingRate);

    // オブジェクト作成
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
        auto enemy = RegisterActor<DummyActor>("Enemy", ActorTag::Enemy);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto wyvern = RegisterActor<WyvernActor>("Wyvern", ActorTag::Enemy);
        wyvern->GetTransform().SetPosition(Vector3(0.0f, 10.0f, -10.0f));
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
    // 負荷テスト
    //for (int i = 0; i < 10; ++i)
    //{
    //    auto player = RegisterActor<PlayerActor>("Player" + std::to_string(i), ActorTag::Player, false);
    //}
    AddCompletionLoading(CompletionLoadingRate);
}
