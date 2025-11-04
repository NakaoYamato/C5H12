#include "SceneCollisionDebug.h"

#include "../../Library/Graphics/Graphics.h"

// コンポーネント
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/ShapeController.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/Rigidbody.h"

#include "../../Source/Enemy/Wyvern/WyvernActor.h"

void SceneCollisionDebug::OnInitialize()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // オブジェクト作成
    ActorManager& actorManager = GetActorManager();
    {
        auto stage = RegisterActor<Actor>("Stage", ActorTag::Stage);

        auto model = stage->LoadModel("./Data/Model/Stage/Land/Land.fbx");
        stage->GetTransform().SetPositionY(-2.7f);
        stage->GetTransform().SetLengthScale(0.1f);
        stage->GetTransform().UpdateTransform(nullptr);
		model.lock()->UpdateTransform(model.lock()->GetPoseNodes(), stage->GetTransform().GetMatrix());

        auto modelCont = stage->AddComponent<ModelRenderer>();
        modelCont->SetRenderType(ModelRenderType::Static);

		stage->AddCollider<MeshCollider>();
    }

    static float Interval = 5.0f;
    for (float f = 0.0f; f <= 10.0f; ++f)
    {
        auto box = RegisterActor<Actor>("box" + std::to_string(f), ActorTag::Player);
		box->AddCollider<BoxCollider>();
		box->GetTransform().SetPositionX(Interval * f);
		box->GetTransform().SetPositionY(0.5f);
        box->AddComponent<Rigidbody>();
	}
    for (float f = 0.0f; f <= 10.0f; ++f)
    {
        auto sphere = RegisterActor<Actor>("sphere" + std::to_string(f), ActorTag::Player);
        sphere->GetTransform().SetPositionX(Interval * f);
        sphere->GetTransform().SetPositionZ(2.0f);
        sphere->GetTransform().SetPositionY(0.5f);
        sphere->AddCollider<SphereCollider>();
        sphere->AddComponent<Rigidbody>();
    }
    for (float f = 0.0f; f <= 10.0f; ++f)
    {
        auto capsule = RegisterActor<Actor>("capsule" + std::to_string(f), ActorTag::Player);
        capsule->GetTransform().SetPositionX(Interval * f);
        capsule->GetTransform().SetPositionZ(-2.0f);
        capsule->GetTransform().SetPositionY(0.5f);
        auto collider = capsule->AddCollider<CapsuleCollider>();
        collider->SetStart(Vector3(0.0f, 0.8f, 0.0f));
        collider->SetEnd(Vector3(0.0f, 3.2f, 0.0f));
        capsule->AddComponent<Rigidbody>();
    }
}
