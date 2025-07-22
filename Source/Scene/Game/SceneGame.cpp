// Winsock2とWinsockの多重定義を避けるために、先にインクルード
#include "../../Source/InGame/NetworkMediator.h"

#include "SceneGame.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Source/AI/MetaAI.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Player/Weapon/Warrior/PlayerSwordActor.h"
#include "../../Source/Player/Weapon/Warrior/PlayerShieldActor.h"
#include "../../Source/Stage/StageActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"
#include "../../Source/Enemy/Gryphus/GryphusActor.h"
#include "../../Source/Enemy/Weak/WeakActor.h"

#include "../../InGame/InputMediator.h"
#include "../../InGame/CanvasMediator.h"
#include "../../Menu/MenuMediator.h"

// ネットワーク有効フラグ
bool SceneGame::NetworkEnabled = false;
// ネットワークアドレス
std::string SceneGame::NetworkAddress = "127.0.0.1";

void SceneGame::OnInitialize()
{
    static const float CompletionLoadingRate = 1.0f / 8.0f;
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
        auto networkMediator = RegisterActor<NetworkMediator>("NetworkMediator", ActorTag::DrawContextParameter);
		// サーバー開始
		if (NetworkEnabled)
		{
			networkMediator->ExecuteServer(NetworkAddress);
		}
        else
        {
            networkMediator->ExecuteServer("");
        }
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto stage0 = RegisterActor<StageActor>("Stage0", ActorTag::Stage, "./Data/Terrain/Save/002.json", Vector3(-50.0f, 0.0f, -50.0f));
    }
    {
        auto stage1 = RegisterActor<StageActor>("Stage1", ActorTag::Stage, "./Data/Terrain/Save/002.json", Vector3(50.0f, 0.0f, -50.0f));
    }
    {
        auto stage2 = RegisterActor<StageActor>("Stage2", ActorTag::Stage, "./Data/Terrain/Save/002.json", Vector3(-50.0f, 0.0f, 50.0f));
    }
    {
        auto stage3 = RegisterActor<StageActor>("Stage3", ActorTag::Stage, "./Data/Terrain/Save/002.json", Vector3(50.0f, 0.0f, 50.0f));
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

    // 予め読み込むモデル
    _preloadModels.push_back(std::make_shared<Model>(device, PlayerActor::GetModelFilePath()));
    _preloadModels.push_back(std::make_shared<Model>(device, PlayerSwordActor::GetModelFilePath()));
    _preloadModels.push_back(std::make_shared<Model>(device, PlayerShieldActor::GetModelFilePath()));
    _preloadModels.push_back(std::make_shared<Model>(device, WyvernActor::GetModelFilePath()));
    _preloadModels.push_back(std::make_shared<Model>(device, GryphusActor::GetModelFilePath()));
    _preloadModels.push_back(std::make_shared<Model>(device, WeakActor::GetModelFilePath()));
    AddCompletionLoading(CompletionLoadingRate);
}
