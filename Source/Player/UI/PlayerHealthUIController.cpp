#include "PlayerHealthUIController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void PlayerHealthUIController::Start()
{
	// シーンからCanvasMediatorを取得
	_canvasMediator = GetActor()->GetScene()->GetActorManager().FindByClass<CanvasMediator>(ActorTag::UI);
	// ユーザーが操作するプレイヤーのHPUIの場合は、CanvasMediatorに登録
	if (_isUserControlled && _canvasMediator.lock())
	{
		_canvasMediator.lock()->SetUserHealthUI(this);
	}
	// ユーザーが操作しないプレイヤーのHPUIの場合は、CanvasMediatorに登録
	else if (!_isUserControlled && _canvasMediator.lock())
	{
		_canvasMediator.lock()->AddOtherUserHealthUI(this);
	}
	// 画像読み込み
	LoadTexture(L"Data/Texture/UI/GaugeFront.png", CenterAlignment::LEFT_CENTER);
	SetPosition(Vector2(30.0f, 40.0f)); // 初期位置を設定
	SetScale(Vector2(1.0f, 1.0f)); // 初期スケールを設定
}
// 削除処理
void PlayerHealthUIController::Deleted()
{
	if (_canvasMediator.lock())
	{
		// ユーザーが操作しないプレイヤーのHPUIの場合は、CanvasMediatorから削除
		if (!_isUserControlled)
		{
			_canvasMediator.lock()->RemoveOtherUserHealthUI(this);
		}
	}
}
// 更新処理
void PlayerHealthUIController::Update(float elapsedTime)
{
	// HPに応じてスケールを変更
	if (_damageable.lock())
	{
		float healthRatio = _damageable.lock()->GetHealth() / _damageable.lock()->GetMaxHealth();
		healthRatio = std::clamp(healthRatio, 0.0f, 1.0f); // 0.0fから1.0fの範囲に制限
		SetScale(Vector2(healthRatio, 1.0f)); // HPに応じて横幅を変更
	}
}
// GUI描画
void PlayerHealthUIController::DrawGui()
{
	ImGui::Text(u8"キャンバス: %s", _canvasMediator.lock() ? u8"有効" : u8"無効");
	UIController::DrawGui();
}
