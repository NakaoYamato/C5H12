#include "SceneDebug.h"

#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Camera/Camera.h"

#include "../../Library/Actor/ActorManager.h"

// コンポーネント
#include "../../Library/Component/Light/LightController.h"
#include "../../Library/Component/Light/PointLightController.h"

#include "../../Library/Component/ShapeController.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

#include "../../Source/Actor/Character/PlayerActor.h"
#include "../../Source/Component/Stage/GrassController.h"

//初期化
void SceneDebug::Initialize()
{
    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();
    // カメラ初期設定
    Camera::Instance().SetPerspectiveFov(
        DirectX::XMConvertToRadians(50),	// 画角
        screenWidth / screenHeight,			// 画面アスペクト比
        0.1f,								// ニアクリップ
        1000.0f								// ファークリップ
    );
    Camera::Instance().SetLookAt(
        { 0, 10, -10 },		// 視点
        { 0, 0, 0 },		// 注視点
        { 0, 1, 0 }			// 上ベクトル
    );
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds", 
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // オブジェクト作成
    {
        std::shared_ptr<Actor> light = ActorManager::Register(u8"Light", ActorTag::DrawContextParameter);
        auto lc = light->AddComponent<LightController>();
        lc->GetLight().SetDirection({ -0.012f,-0.819f,0.574f, 0.0f });
    }
    {
        std::shared_ptr<Actor> light = ActorManager::Register(u8"PointLight0", ActorTag::DrawContextParameter);
        light->AddComponent<PointLightController>();
    }
    {
        std::shared_ptr<Actor> light = ActorManager::Register(u8"PointLight1", ActorTag::DrawContextParameter);
        light->AddComponent<PointLightController>();
    }
    {
        auto stage = ActorManager::Register("Stage", ActorTag::Stage);

        //stage->GetTransform().SetPositionY(-2.7f);
        //stage->GetTransform().SetLengthScale(0.1f);
        //auto modelCont = stage->AddComponent<ModelRenderer>("./Data/Model/Stage/Land/Land.fbx");

        stage->GetTransform().SetLengthScale(0.01f);
        auto modelCont = stage->AddComponent<ModelRenderer>("./Data/Model/Stage/Test/test.fbx");

        modelCont->SetRenderType(ModelRenderType::Static);
    }
    {
        //auto stage = ActorManager::Create("StageGrass", ActorTag::Stage);
        //stage->GetTransform().SetPositionY(-2.7f);
        //stage->GetTransform().SetLengthScale(0.1f);
        //auto modelCont = stage->AddComponent<ModelRenderer>("./Data/Model/Stage/Land/Land.fbx");
        //modelCont->SetRenderType(ModelRenderType::Static);
        //modelCont->SetShader("Grass");
        //modelCont->SetColor(VECTOR4_GREEN);
        //auto grassController = stage->AddComponent<GrassController>();
    }
    {
        auto player = ActorManager::Register<PlayerActor>("Player", ActorTag::Player);
    }
    {
        //auto box = ActorManager::Register("box", ActorTag::Player);
        //auto boxC = box->AddComponent<ShapeController>();
    }
}

//終了化 
void SceneDebug::Finalize()
{
    ActorManager::Clear();
}
