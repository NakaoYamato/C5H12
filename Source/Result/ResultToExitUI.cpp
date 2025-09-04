#include "ResultToExitUI.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>

// 開始処理
void ResultToExitUI::Start()
{
	LoadTexture("Button",
		L"./Data/Texture/Result/Exit.png",
		Sprite::CenterAlignment::CenterCenter);
	SetPosition("Button", Vector2(640.0f, 400.0f)); // 初期位置を設定
}
// 更新処理
void ResultToExitUI::Update(float elapsedTime)
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
			// アプリ終了
			PostMessage(Graphics::Instance().GetHwnd(), WM_CLOSE, 0, 0);
		}
	}
	else
	{
		SetScale("Button", Vector2::One);
	}
}
// GUI描画
void ResultToExitUI::DrawGui()
{
	SpriteRenderer::DrawGui();
}
