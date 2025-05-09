#include "ScenePlayerDebug.h"

#include "../../Library/Graphics/Graphics.h"

// コンポーネント
#include "../../Library/Component/Light/PointLightController.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Source/Actor/Player/PlayerActor.h"

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
        auto stage = RegisterActor<Actor>("Stage", ActorTag::Stage);

        stage->LoadModel("./Data/Model/Stage/Land/Land.fbx");
        stage->GetTransform().SetPositionY(-2.7f);
        stage->GetTransform().SetLengthScale(0.1f);
        auto modelCont = stage->AddComponent<ModelRenderer>();

        modelCont->SetRenderType(ModelRenderType::Static);
    }
    {
        auto player = RegisterActor<PlayerActor>("Player", ActorTag::Player);
    }
}
