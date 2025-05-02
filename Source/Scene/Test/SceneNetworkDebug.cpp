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

    // オブジェクト作成
    ActorManager& actorManager = GetActorManager();

    _client = std::make_shared<ClientAssignment>();
    _client->Execute();
}

void SceneNetworkDebug::Update(float elapsedTime)
{
    Scene::Update(elapsedTime);

    _client->Update();
}

void SceneNetworkDebug::Finalize()
{
    _client->Exit();
    Scene::Finalize();
}

void SceneNetworkDebug::DrawGui()
{
    Scene::DrawGui();
    _client->DrawGui();
}
