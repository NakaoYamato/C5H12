#include "SceneCollisionDebug.h"

#include "../../Library/Graphics/Graphics.h"

// コンポーネント
#include "../../Library/Component/Light/PointLightController.h"
#include "../../Library/Component/ShapeController.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Source/Actor/Enemy/Dragon/DragonActor.h"

void SceneCollisionDebug::Initialize()
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

		stage->AddCollider<MeshCollider>();
    }
    for (float f = 0.0f; f <= 10.0f; ++f)
    {
        auto box = RegisterActor<Actor>("box" + std::to_string(f), ActorTag::Player);
        auto boxC = box->AddComponent<ShapeController>();
		box->AddCollider<BoxCollider>();
		box->GetTransform().SetPositionX(2.0f * f);
		box->GetTransform().SetPositionY(0.5f);
		boxC->SetType(ShapeType::Box);
	}
    for (float f = 0.0f; f <= 10.0f; ++f)
    {
        auto sphere = RegisterActor<Actor>("sphere" + std::to_string(f), ActorTag::Player);
        auto sphereC = sphere->AddComponent<ShapeController>(ShapeType::Sphere);
        sphere->GetTransform().SetPositionX(2.0f * f);
        sphere->GetTransform().SetPositionZ(2.0f);
        sphere->GetTransform().SetPositionY(0.5f);
        sphere->AddCollider<SphereCollider>();
    }
    for (float f = 0.0f; f <= 10.0f; ++f)
    {
        auto capsule = RegisterActor<Actor>("capsule" + std::to_string(f), ActorTag::Player);
        auto capsuleC = capsule->AddComponent<ShapeController>(ShapeType::Capsule);
        capsule->GetTransform().SetPositionX(2.0f * f);
        capsule->GetTransform().SetPositionZ(-2.0f);
        capsule->GetTransform().SetPositionY(0.5f);
        capsule->AddCollider<CapsuleCollider>();
    }
    {
        //auto dragon = RegisterActor<DragonActor>("Dragon", ActorTag::Enemy);
    }
}
