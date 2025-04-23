#include "ScenePlayerDebug.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Camera/Camera.h"

// コンポーネント
#include "../../Library/Component/Light/PointLightController.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Source/Actor/Player/PlayerActor.h"

void ScenePlayerDebug::Initialize()
{
    Scene::Initialize();

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
    ActorManager& actorManager = GetActorManager();
    {
        auto stage = RegisterActor<Actor>("Stage", ActorTag::Stage);

        //stage->GetTransform().SetPositionY(-2.7f);
        //stage->GetTransform().SetLengthScale(0.1f);
        //auto modelCont = stage->AddComponent<ModelRenderer>("./Data/Model/Stage/Land/Land.fbx");

        stage->GetTransform().SetLengthScale(0.01f);
        auto modelCont = stage->AddComponent<ModelRenderer>("./Data/Model/Stage/Test/test.fbx");

        modelCont->SetRenderType(ModelRenderType::Static);
    }
    {
        auto player = RegisterActor<PlayerActor>("Player", ActorTag::Player);
    }
}
