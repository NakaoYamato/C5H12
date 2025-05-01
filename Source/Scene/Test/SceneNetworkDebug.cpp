#include "SceneNetworkDebug.h"

#include "../../Library/Graphics/Graphics.h"

//初期化
void SceneNetworkDebug::Initialize()
{
    Scene::Initialize();

    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // サーバー作成
    _server = std::make_shared<ServerAssignment>();
    // サーバー起動
    //_server->Execute();

    // オブジェクト作成
    ActorManager& actorManager = GetActorManager();
}

// 終了化
void SceneNetworkDebug::Finalize()
{
    Scene::Finalize();

    // サーバー終了
    _server->Exit();
}
