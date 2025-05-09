#include "SceneDebug.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/JobSystem/JobSystem.h"

// コンポーネント
#include "../../Library/Component/Light/PointLightController.h"

#include "../../Library/Component/ShapeController.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

#include "../../Source/Actor/Player/PlayerActor.h"
#include "../../Source/Component/Stage/GrassController.h"

#include "../../Source/Actor/Dragon/DragonActor.h"

//初期化
void SceneDebug::Initialize()
{
    Scene::Initialize();

    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds", 
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // オブジェクト作成
    ActorManager& actorManager = GetActorManager();
    // ジョブシステムのテスト
    if (false)
    {
        // あんま早くならん
        std::vector<std::future<void>> jobResults;

        jobResults.emplace_back(JobSystem::Instance().EnqueueJob("Light",
            ImGuiControl::Profiler::Color::Blue,
            [&]()
            {
                std::shared_ptr<Actor> light = RegisterActor<Actor>(u8"PointLight0", ActorTag::DrawContextParameter);
                light->AddComponent<PointLightController>();
            }
        ));
        //jobResults.emplace_back(JobSystem::Instance().EnqueueJob("Light",
        //    ImGuiControl::Profiler::Color::Blue,
        //    [&]()
        //    {
        //        std::shared_ptr<Actor> light = RegisterActor<Actor>(u8"PointLight1", ActorTag::DrawContextParameter);
        //        light->AddComponent<PointLightController>();
        //    }
        //));
        jobResults.emplace_back(JobSystem::Instance().EnqueueJob("Stage",
            ImGuiControl::Profiler::Color::Dark,
            [&]()
            {
                auto stage = RegisterActor<Actor>("Stage", ActorTag::Stage);

                //stage->LoadModel("./Data/Model/Stage/Land/Land.fbx");
                //stage->GetTransform().SetPositionY(-2.7f);
                //stage->GetTransform().SetLengthScale(0.1f);
                //auto modelCont = stage->AddComponent<ModelRenderer>();

                stage->LoadModel("./Data/Model/Stage/Test/test.fbx");
                stage->GetTransform().SetLengthScale(0.01f);
                auto modelCont = stage->AddComponent<ModelRenderer>();

                modelCont->SetRenderType(ModelRenderType::Static);
            }
        ));
        jobResults.emplace_back(JobSystem::Instance().EnqueueJob("Player",
            ImGuiControl::Profiler::Color::Green,
            [&]()
            {
                auto player = RegisterActor<PlayerActor>("Player", ActorTag::Player);
            }
        ));

        // 検証結果:DragonActor::OnCreate()にて複数スレッドでテクスチャにアクセスするとエラー
        jobResults.emplace_back(JobSystem::Instance().EnqueueJob("Dragon",
            ImGuiControl::Profiler::Color::Purple,
            [&]()
            {
                auto dragon = RegisterActor<DragonActor>("Dragon", ActorTag::Enemy);
            }
        ));
        jobResults.emplace_back(JobSystem::Instance().EnqueueJob("Dragon",
            ImGuiControl::Profiler::Color::Purple,
            [&]()
            {
                auto dragon1 = RegisterActor<DragonActor>("Dragon1", ActorTag::Enemy);
            }
        ));
        jobResults.emplace_back(JobSystem::Instance().EnqueueJob("Dragon",
            ImGuiControl::Profiler::Color::Purple,
            [&]()
            {
                auto dragon2 = RegisterActor<DragonActor>("Dragon2", ActorTag::Enemy);
            }
        ));

        // すべてのジョブの終了を待機
        for (auto& result : jobResults)
        {
            result.get();
        }
    }
    else
    {
        {
            std::shared_ptr<Actor> light = RegisterActor<Actor>(u8"PointLight0", ActorTag::DrawContextParameter);
            light->AddComponent<PointLightController>();
        }
        {
            std::shared_ptr<Actor> light = RegisterActor<Actor>(u8"PointLight1", ActorTag::DrawContextParameter);
            light->AddComponent<PointLightController>();
        }
        {
            std::shared_ptr<Actor> light = RegisterActor<Actor>(u8"PointLight2", ActorTag::DrawContextParameter);
            light->AddComponent<PointLightController>();
        }
        {
            std::shared_ptr<Actor> light = RegisterActor<Actor>(u8"PointLight3", ActorTag::DrawContextParameter);
            light->AddComponent<PointLightController>();
        }
        {
            auto stage = RegisterActor<Actor>("Stage", ActorTag::Stage);

            //stage->LoadModel("./Data/Model/Stage/Land/Land.fbx");
            //stage->GetTransform().SetPositionY(-2.7f);
            //stage->GetTransform().SetLengthScale(0.1f);
            //auto modelCont = stage->AddComponent<ModelRenderer>();

            stage->LoadModel("./Data/Model/Stage/Test/test.fbx");
            stage->GetTransform().SetLengthScale(0.01f);
            auto modelCont = stage->AddComponent<ModelRenderer>();

            modelCont->SetRenderType(ModelRenderType::Static);
        }
        {
            //auto stage = actorManager.Create("StageGrass", ActorTag::Stage);
            //stage->GetTransform().SetPositionY(-2.7f);
            //stage->GetTransform().SetLengthScale(0.1f);
            //auto modelCont = stage->AddComponent<ModelRenderer>("./Data/Model/Stage/Land/Land.fbx");
            //modelCont->SetRenderType(ModelRenderType::Static);
            //modelCont->SetShader("Grass");
            //modelCont->SetColor(VECTOR4_GREEN);
            //auto grassController = stage->AddComponent<GrassController>();
        }

        {
            auto player = RegisterActor<PlayerActor>("Player", ActorTag::Player);
        }
        {
            auto box = RegisterActor<Actor>("box", ActorTag::Player);
            auto boxC = box->AddComponent<ShapeController>();
        }

        {
            auto dragon = RegisterActor<DragonActor>("Dragon", ActorTag::Enemy);
        }
        {
            auto dragon = RegisterActor<DragonActor>("Dragon1", ActorTag::Enemy);
        }
        {
            auto dragon = RegisterActor<DragonActor>("Dragon2", ActorTag::Enemy);
        }
    }
}
