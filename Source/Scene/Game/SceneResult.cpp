#include "SceneResult.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Source/Result/ResultToExitUI.h"
#include "../../Source/Result/ResultToTitleUI.h"

// 初期化
void SceneResult::OnInitialize()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    {
		auto background = RegisterActor<Actor>("Background", ActorTag::UI);
		auto spriteRenderer = background->AddComponent<SpriteRenderer>();
		// TODO : 背景画像のパスを適切なものに変更する
		spriteRenderer->LoadTexture("TitleBackground", L"./Data/Texture/Result/Background.png", Sprite::CenterAlignment::LeftUp);
    }
	{
		auto resultToExitUI = RegisterActor<Actor>("ResultToExitUI", ActorTag::UI);
		resultToExitUI->AddComponent<ResultToExitUI>();
	}
	{
		auto resultToTitleUI = RegisterActor<Actor>("ResultToTitleUI", ActorTag::UI);
		resultToTitleUI->AddComponent<ResultToTitleUI>();
	}
}
