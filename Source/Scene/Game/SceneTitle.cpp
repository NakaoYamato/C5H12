#include "SceneTitle.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Component/SpriteRenderer.h"
#include "../../Source/Title/TitleUI.h"

// èâä˙âª
void SceneTitle::OnInitialize()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    {
		auto background = RegisterActor<Actor>("Background", ActorTag::UI);
		auto spriteRenderer = background->AddComponent<SpriteRenderer>();
		spriteRenderer->LoadTexture("TitleBackground", L"./Data/Texture/Title/Background.png", SpriteRenderer::CenterAlignment::LEFT_UP);
    }
    {
        auto startButton = RegisterActor<Actor>("StartButton", ActorTag::UI);
        auto spriteRenderer = startButton->AddComponent<SpriteRenderer>();
        spriteRenderer->LoadTexture("StartButton", L"./Data/Texture/Title/Start.png", SpriteRenderer::CenterAlignment::CENTER_CENTER);
        spriteRenderer->SetPosition("StartButton", Vector2(600.0f, 600.0f));
        auto titleUI = startButton->AddComponent<TitleUI>();
    }
}
