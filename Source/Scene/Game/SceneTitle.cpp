#include "SceneTitle.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Component/SpriteRenderer.h"

#include "../../Source/Mediator/TitleMediator.h"

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
		spriteRenderer->LoadTexture("TitleBackground", L"./Data/Texture/Title/Background.png", Sprite::CenterAlignment::LeftUp);
    }
    {
		auto titleMediator = RegisterActor<TitleMediator>("TitleMediator", ActorTag::UI);
    }
}
