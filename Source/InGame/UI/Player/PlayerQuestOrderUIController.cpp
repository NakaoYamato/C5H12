#include "PlayerQuestOrderUIController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>

// 開始処理
void PlayerQuestOrderUIController::Start()
{
	_spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		if (!spriteRenderer->IsLoaded())
		{
			// 画像読み込み
			spriteRenderer->LoadTexture(FrameSpr, L"", Sprite::CenterAlignment::CenterCenter);
		}
	}

	// GameManagerからQuestControllerを取得
	auto gameManager = GetActor()->GetScene()->GetActorManager().FindByName("GameManager", ActorTag::System);
	if (gameManager)
	{
		_questController = gameManager->GetComponent<QuestController>();
	}
}

// 更新処理
void PlayerQuestOrderUIController::Update(float elapsedTime)
{
	auto actor = _playerActor.lock();
	auto spriteRenderer = _spriteRenderer.lock();
	auto questController = _questController.lock();
	if (!actor || !spriteRenderer || !questController)
		return;

	// クエスト受注中でなければ非表示にする
	if (questController->GetCurrentState() != QuestController::State::Accepted)
	{
		spriteRenderer->SetActive(FrameSpr, false);
		return;
	}
	else
	{
		spriteRenderer->SetActive(FrameSpr, true);
	}
	
	// プレイヤーの位置に追従
	Vector3 playerPos = actor->GetTransform().GetWorldPosition();
	playerPos.y += _offsetY;
	// ワールド座標をスクリーン座標に変換
	Vector3 screenPos = playerPos.Project(
		Graphics::Instance().GetScreenWidth(),
		Graphics::Instance().GetScreenHeight(),
		this->GetActor()->GetScene()->GetMainCamera()->GetView(),
		this->GetActor()->GetScene()->GetMainCamera()->GetProjection());
	spriteRenderer->GetRectTransform(FrameSpr).SetLocalPosition(Vector2(screenPos.x, screenPos.y));

	// 画面外かカメラの後ろにいる場合は透明にする
	spriteRenderer->SetColorAlpha(FrameSpr, screenPos.x < 0.0f ? 0.0f : 1.0f);
}

// GUI描画
void PlayerQuestOrderUIController::DrawGui()
{
	ImGui::DragFloat(u8"オフセットY", &_offsetY, 0.1f, -10.0f, 10.0f);
}
