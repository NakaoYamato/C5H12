#include "SceneNetworkDebug.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Source/Actor/GameSystem/GameSystemActor.h"

//初期化
void SceneNetworkDebug::Initialize()
{
    Scene::Initialize();

    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // オブジェクト作成
    {
        auto gameSystem = RegisterActor<GameSystemActor>("GameSystem", ActorTag::DrawContextParameter);
    }
}
