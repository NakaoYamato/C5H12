#include "PlayerBuffUIController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/InGame/InGameCanvasActor.h"

#include <imgui.h>

// 開始処理
void PlayerBuffUIController::Start()
{
	// キャンバスがあるか確認
	if (auto canvas = GetActor()->GetScene()->GetActorManager().FindByClass<InGameCanvasActor>(ActorTag::UI))
	{
		// 親設定
		GetActor()->SetParent(canvas.get());
	}

	_buffIconSprite.LoadTexture(L"Data/Texture/UI/Player/BuffIcon.png", Sprite::CenterAlignment::CenterCenter);
	_buffIconSprite.SetTexSize(Vector2(256.0f, 256.0f));
}

// 更新処理
void PlayerBuffUIController::Update(float elapsedTime)
{
}

// 3D描画後の描画処理
void PlayerBuffUIController::DelayedRender(const RenderContext& rc)
{
	auto buffController = _buffController.lock();
	if (!buffController)
		return;

	// バフ描画
	Vector2 iconPos = _buffIconStartPos;
	for (auto& buff : buffController->GetActiveBuffs())
	{
		_buffIconSprite.GetRectTransform().SetLocalPosition(iconPos);
		_buffIconSprite.UpdateTransform();
		switch (buff->_type)
		{
		case BuffController::BuffType::Attack:
			_buffIconSprite.SetTexPos(Vector2(0.0f, 0.0f));
			break;
		case BuffController::BuffType::Defense:
			_buffIconSprite.SetTexPos(Vector2(256.0f, 0.0f));
			break;
		}

		_buffIconSprite.Render(rc, GetActor()->GetScene()->GetTextureRenderer());

		iconPos.x += _buffIconSpacing;
	}
}

// GUI描画
void PlayerBuffUIController::DrawGui()
{
	_buffIconSprite.DrawGui();
	ImGui::DragFloat2(u8"バフアイコン開始位置", &_buffIconStartPos.x, 1.0f);
	ImGui::DragFloat(u8"バフアイコン間隔", &_buffIconSpacing, 1.0f);
}
