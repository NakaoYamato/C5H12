// Winsock2とWinsockの多重定義を避けるために、先にインクルード
#include "../../Source/Network/NetworkMediator.h"

#include "SceneNetworkDebug.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Actor/Stage/StageManager.h"

#include "../../InGame/InGameCanvasActor.h"
#include "../../InGame/GameManager.h"

#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"

//初期化
void SceneNetworkDebug::OnInitialize()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // オブジェクト作成
    {
        auto gameManager = RegisterActor<GameManager>("GameManager", ActorTag::System);
    }
    {
        auto networkMediator = RegisterActor<NetworkMediator>("NetworkMediator", ActorTag::System);
    }
    {
        RegisterActor<StageManager>("StageManager", ActorTag::Stage);
    }
    {
        auto canvas = RegisterActor<InGameCanvasActor>("Canvas", ActorTag::UI);
    }

    // 予め読み込むモデル
    _preloadModels.push_back(std::make_shared<Model>(device, PlayerActor::GetModelFilePath()));
    _preloadModels.push_back(std::make_shared<Model>(device, WyvernActor::GetModelFilePath()));
}
