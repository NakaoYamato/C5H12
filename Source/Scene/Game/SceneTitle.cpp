#include "SceneTitle.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Component/SpriteRenderer.h"
#include "../../Library/Actor/Terrain/TerrainActor.h"
#include "../../Library/Actor/UI/UIActor.h"

#include "../../Source/Title/Wyvern/TitleWyvernActor.h"
#include "../../Source/Title/UI/TitleUIcontroller.h"

// 初期化
void SceneTitle::OnInitialize()
{
    static const float CompletionLoadingRate = 1.0f / 3.0f;

    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // カメラの初期化
    GetMainCameraActor()->SetLookAt(
        Vector3(0.0f, 1.0f, -7.0f),
        Vector3(1.2f, 2.5f, 3.0f),
        Vector3(0.0f, 1.0f, 0.0f)
    );

    {
		auto background = RegisterActor<Actor>("Background", ActorTag::UI);
		auto spriteRenderer = background->AddComponent<SpriteRenderer>();
		spriteRenderer->LoadTexture("TitleBackground", L"./Data/Texture/Title/Background.png", Sprite::CenterAlignment::LeftUp);
    }
    {
        auto stage0 = RegisterActor<TerrainActor>("Stage0", ActorTag::Stage, "./Data/Terrain/Save/001.json", Vector3(0.0f, 0.0f, 0.0f));
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto wyvern = RegisterActor<TitleWyvernActor>("Wyvern", ActorTag::Enemy);
        wyvern->GetTransform().SetPosition(Vector3(0.0f, 0.23f, 7.0f));
        wyvern->GetTransform().SetAngleY(DirectX::XMConvertToRadians(180.0f));
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto uiActor = RegisterActor<UIActor>("TitleUI", ActorTag::UI);
        auto titleUIcontroller = uiActor->AddComponent<TitleUIcontroller>();
    }
    AddCompletionLoading(CompletionLoadingRate);


    // デバッグGUI表示オフ
    Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F6;
    // デバック当たり判定表示オフ
    Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F7;
}

// 更新処理
void SceneTitle::OnUpdate(float elapsedTime)
{
    // カメラ
    GetMainCameraActor()->SetLookAt(
        Vector3(0.0f, 1.0f, -7.0f),
        Vector3(1.2f, 2.5f, 3.0f),
        Vector3(0.0f, 1.0f, 0.0f)
    );
}
