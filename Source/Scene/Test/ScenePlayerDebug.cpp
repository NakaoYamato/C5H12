#include "ScenePlayerDebug.h"

#include "../../Library/Graphics/Graphics.h"

// コンポーネント
#include "../../Library/Component/Light/PointLightController.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Stage/StageActor.h"
#include "../../Source/Enemy/Dummy/DummyActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"

void ScenePlayerDebug::Initialize()
{
    Scene::Initialize();

    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // オブジェクト作成
    ActorManager& actorManager = GetActorManager();
    {
        auto stage = RegisterActor<StageActor>("Stage", ActorTag::Stage);
    }
    {
        auto player = RegisterActor<PlayerActor>("Player", ActorTag::Player);
    }
    {
        auto enemy = RegisterActor<DummyActor>("Enemy", ActorTag::Enemy);
    }
    {
        auto wyvern = RegisterActor<WyvernActor>("Wyvern", ActorTag::Enemy);
        wyvern->GetTransform().SetPosition(Vector3(0.0f, 10.0f, -10.0f));
    }
    // 負荷テスト
    //for (int i = 0; i < 10; ++i)
    //{
    //    auto player = RegisterActor<PlayerActor>("Player" + std::to_string(i), ActorTag::Player, false);
    //}
}
