// Winsock2とWinsockの多重定義を避けるために、先にインクルード
#include "../../Source/Network/NetworkMediator.h"

#include "SceneGame.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Actor/Stage/StageManager.h"

#include "../../Source/AI/MetaAI.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"
#include "../../Source/Stage/StageController.h"
#include "../../Source/Stage/Props/Chest/ChestActor.h"
#include "../../Source/Stage/Props/Fence/FenceActor.h"

#include "../../InGame/InputManager.h"
#include "../../InGame/InGameCanvasActor.h"

// ネットワーク有効フラグ
bool SceneGame::NetworkEnabled = false;
// ネットワークアドレス
std::string SceneGame::NetworkAddress = "127.0.0.1";

void SceneGame::OnInitialize()
{
    static const float CompletionLoadingRate = 1.0f / 6.0f;
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");
    AddCompletionLoading(CompletionLoadingRate);

    // 予め読み込むモデル
    _preloadModels.push_back(std::make_shared<Model>(device, PlayerActor::GetModelFilePath()));
    _preloadModels.push_back(std::make_shared<Model>(device, WyvernActor::GetModelFilePath()));
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
        RegisterActor<StageManager>("StageManager", ActorTag::Stage);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto canvas = RegisterActor<InGameCanvasActor>("Canvas", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);
}
