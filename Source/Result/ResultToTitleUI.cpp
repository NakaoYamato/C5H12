#include "ResultToTitleUI.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Scene/SceneManager.h"

#include <imgui.h>

// 名前取得
void ResultToTitleUI::Start()
{
	LoadTexture("Button",
		L"./Data/Texture/Result/ToTitle.png",
		Sprite::CenterAlignment::CenterCenter);
	SetPosition("Button", Vector2(640.0f, 550.0f)); // 初期位置を設定
}
// 更新処理
void ResultToTitleUI::Update(float elapsedTime)
{
	Vector2 mousePos{};
	mousePos.x = _INPUT_VALUE("MousePositionX");
	mousePos.y = _INPUT_VALUE("MousePositionY");
	if (IsHit("Button", mousePos))
	{
		// マウスカーソルが当たっている場合、拡大
		SetScale("Button", Vector2(1.1f, 1.1f));
		// 入力処理
		if (_INPUT_RELEASED("OK"))
		{
			// タイトルへ遷移
			SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Title");
		}
	}
	else
	{
		SetScale("Button", Vector2::One);
	}
}
// GUI描画
void ResultToTitleUI::DrawGui()
{
	SpriteRenderer::DrawGui();
}
