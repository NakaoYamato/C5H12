#include "SceneDebug.h"

#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Camera/Camera.h"

#include "../../Library/Actor/ActorManager.h"

// コンポーネント
#include "../../Library/Component/Light/LightController.h"
#include "../../Library/Component/Light/PointLightController.h"

#include "../../Library/Component/ShapeController.h"
#include "../../Library/Component/ModelController.h"
#include "../../Library/Component/Animator.h"

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

    SetSkyMap(L"./Data/SkyMap/S0.dds");

    // オブジェクト作成
    {
        std::shared_ptr<Actor> light = ActorManager::Create(u8"Light", ActorTag::DrawContextParameter);
        auto lc = light->AddComponent<LightController>();
        lc->GetLight().SetDirection({ -0.012f,-0.819f,0.574f, 0.0f });
    }
    {
        std::shared_ptr<Actor> light = ActorManager::Create(u8"PointLight0", ActorTag::DrawContextParameter);
        light->AddComponent<PointLightController>();
    }
    {
        std::shared_ptr<Actor> light = ActorManager::Create(u8"PointLight1", ActorTag::DrawContextParameter);
        light->AddComponent<PointLightController>();
    }
    {
        auto stage = ActorManager::Create("Stage", ActorTag::Stage);
        stage->GetTransform().SetPositionY(-2.0f);
        auto modelCont = stage->AddComponent<ModelController>("./Data/Model/Stage/Land/Land1.fbx");
    }
    {
        auto player = ActorManager::Create("Player", ActorTag::Player);
        player->GetTransform().SetLengthScale(0.01f);
        auto modelCont = player->AddComponent<ModelController>("./Data/Model/Player/HPmaidEyeBone.fbx");
        auto animator = player->AddComponent<Animator>(modelCont->GetModel());
    }
}

//終了化 
void SceneDebug::Finalize()
{
    ActorManager::Clear();
}
