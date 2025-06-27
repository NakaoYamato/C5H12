// Winsock2とWinsockの多重定義を避けるために、先にインクルード
#include "../../Source/InGame/NetworkMediator.h"

#include "SceneNetworkDebug.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Source/AI/MetaAI.h"
#include "../../Source/Stage/StageActor.h"
#include "../../InGame/CanvasMediator.h"
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
        auto metaAI = RegisterActor<Actor>("MetaAI", ActorTag::DrawContextParameter);
        metaAI->AddComponent<MetaAI>();
    }
    {
        auto networkMediator = RegisterActor<NetworkMediator>("NetworkMediator", ActorTag::DrawContextParameter);
    }
    {
        auto stage = RegisterActor<StageActor>("Stage", ActorTag::Stage);
    }
    {
        auto canvas = RegisterActor<CanvasMediator>("CanvasMediator", ActorTag::UI);
    }

    // 予め読み込むモデル
    _preloadModels.push_back(std::make_shared<Model>(device, PlayerActor::GetModelFilePath()));
    _preloadModels.push_back(std::make_shared<Model>(device, WyvernActor::GetModelFilePath()));
}
