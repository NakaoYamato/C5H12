#include "ScenePlayerDebug.h"

#include "../../Library/Graphics/Graphics.h"

// コンポーネント
#include "../../Source/AI/MetaAI.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Stage/StageActor.h"
#include "../../Source/Enemy/Dummy/DummyActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"
#include "../../Source/Enemy/Weak/WeakActor.h"
#include "../../Source/Enemy/Gryphus/GryphusActor.h"

#include "../../InGame/InputMediator.h"
#include "../../InGame/CanvasMediator.h"
#include "../../Menu/MenuMediator.h"

void ScenePlayerDebug::OnInitialize()
{
	static const float CompletionLoadingRate = 1.0f / 9.0f;
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
        auto stage0 = RegisterActor<StageActor>("Stage0", ActorTag::Stage, 0, "./Data/Terrain/Save/002.json", Vector3(0.0f, 0.0f, 0.0f));
    }
    //{
    //    auto stage1 = RegisterActor<StageActor>("Stage1", ActorTag::Stage, 1, "./Data/Terrain/Save/002.json", Vector3(50.0f, 0.0f, -50.0f));
    //}
    //{
    //    auto stage2 = RegisterActor<StageActor>("Stage2", ActorTag::Stage, 2, "./Data/Terrain/Save/002.json", Vector3(-50.0f, 0.0f, 50.0f));
    //}
    //{
    //    auto stage3 = RegisterActor<StageActor>("Stage3", ActorTag::Stage, 3, "./Data/Terrain/Save/002.json", Vector3(50.0f, 0.0f, 50.0f));
    //}
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto player = RegisterActor<PlayerActor>("Player", ActorTag::Player);
    }
    //AddCompletionLoading(CompletionLoadingRate);
    //{
    //    auto wyvern = RegisterActor<WyvernActor>("Wyvern", ActorTag::Enemy);
    //    wyvern->GetTransform().SetPosition(Vector3(0.0f, 10.0f, -10.0f));
    //}
    AddCompletionLoading(CompletionLoadingRate);
    {
		auto menu = RegisterActor<MenuMediator>("MenuMediator", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto canvas = RegisterActor<CanvasMediator>("CanvasMediator", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);
    //{
    //    auto weak0 = RegisterActor<WeakActor>("Weak0", ActorTag::Enemy);
    //}
    //{
    //    auto weak1 = RegisterActor<WeakActor>("Weak1", ActorTag::Enemy);
    //    weak1->GetTransform().SetPosition(Vector3(0.0f, 5.0f, -5.0f));
    //}
    //{
    //    auto gryphus = RegisterActor<GryphusActor>("Gryphus", ActorTag::Enemy);
    //    gryphus->GetTransform().SetPosition(Vector3(0.0f, 5.0f, -40.0f));
    //}
    //AddCompletionLoading(CompletionLoadingRate);
    // 負荷テスト
    //for (int i = 0; i < 10; ++i)
    //{
    //    auto player = RegisterActor<PlayerActor>("Player" + std::to_string(i), ActorTag::Player, false);
    //}
    //AddCompletionLoading(CompletionLoadingRate);
}
