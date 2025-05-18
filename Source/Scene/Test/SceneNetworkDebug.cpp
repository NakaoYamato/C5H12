// Winsock2とWinsockの多重定義を避けるために、先にインクルード
#include "../../Source/Actor/Mediator/NetworkMediator.h"

#include "SceneNetworkDebug.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Source/Actor/Stage/StageActor.h"
#include "../../Source/Actor/Enemy/Dummy/DummyActor.h"

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
        auto networkMediator = RegisterActor<NetworkMediator>("NetworkMediator", ActorTag::DrawContextParameter);
    }
    {
        auto stage = RegisterActor<StageActor>("Stage", ActorTag::Stage);
    }
    {
        auto enemy = RegisterActor<DummyActor>("Enemy", ActorTag::Enemy);
    }
}
