// Winsock2とWinsockの多重定義を避けるために、先にインクルード
#include "../../Source/Network/NetworkMediator.h"

#include "SceneGame.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Actor/Terrain/TerrainActor.h"

#include "../../Source/AI/MetaAI.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"
#include "../../Source/Stage/StageController.h"
#include "../../Source/Stage/Props/Chest/ChestActor.h"

#include "../../InGame/InputManager.h"
#include "../../InGame/InGameCanvasActor.h"
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
        auto metaAI = RegisterActor<Actor>("MetaAI", ActorTag::System);
        metaAI->AddComponent<MetaAI>();
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto inputManager = RegisterActor<InputManager>("InputManager", ActorTag::System);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto networkMediator = RegisterActor<NetworkMediator>("NetworkMediator", ActorTag::System);
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
        auto stage0 = RegisterActor<TerrainActor>("Stage0", ActorTag::Stage, "./Data/Terrain/Save/001.json", Vector3(0.0f, 0.0f, 0.0f));
        auto stageController = stage0->AddComponent<StageController>(0);
    }
    {
        auto stage1 = RegisterActor<TerrainActor>("Stage1", ActorTag::Stage, "./Data/Terrain/Save/002.json", Vector3(100.0f, 0.0f, 0.0f));
        auto stageController = stage1->AddComponent<StageController>(1);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto Chest = RegisterActor<ChestActor>("Chest", ActorTag::Stage);
        Chest->GetTransform().SetPosition(Vector3(-43.0f, 0.0f, 9.0f));
        Chest->GetTransform().SetAngleY(DirectX::XMConvertToRadians(90.0f));
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto menu = RegisterActor<MenuMediator>("MenuMediator", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto canvas = RegisterActor<InGameCanvasActor>("Canvas", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);

    // 予め読み込むモデル
    _preloadModels.push_back(std::make_shared<Model>(device, PlayerActor::GetModelFilePath()));
    _preloadModels.push_back(std::make_shared<Model>(device, WyvernActor::GetModelFilePath()));
    AddCompletionLoading(CompletionLoadingRate);
}
