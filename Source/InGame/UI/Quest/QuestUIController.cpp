#include "QuestUIController.h"

#include "../../Library/Scene/Scene.h"

#include <Mygui.h>

// 開始処理
void QuestUIController::Start()
{
	// ゲームマネージャーからクエスト受注コントローラー取得
	auto gameManager = GetActor()->GetScene()->GetActorManager().FindByName("GameManager", ActorTag::System);
	if (gameManager)
	{
		if (auto cont = gameManager->GetComponent<QuestController>())
		{
			_questController = cont;
		}
	}

	_spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();
	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		if (!spriteRenderer->IsLoaded())
		{
			spriteRenderer->LoadTexture(Front, L"");
		}
	}
}

// 更新処理
void QuestUIController::Update(float elapsedTime)
{
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;
	auto questController = _questController.lock();
	if (!questController)
		return;

	// クエスト受注状態に応じて処理を変える
	switch (questController->GetCurrentState())
	{
	case QuestController::State::Accepted:
		spriteRenderer->SetActive(Front, true);
		break;
	case QuestController::State::InQuest:
		spriteRenderer->SetActive(Front, true);

		break;
	default:
		// UIを非表示にする
		spriteRenderer->SetActive(Front, false);
		break;
	}
}

// GUI描画
void QuestUIController::DrawGui()
{
	ImGui::DragFloat2(u8"タイトル位置", &_titlePosition.x, 1.0f);
	ImGui::ColorEdit4(u8"タイトル色", &_titleColor.x);
	ImGui::DragFloat2(u8"タイトルスケール", &_titleScale.x, 0.1f);
	ImGui::Separator();
	ImGui::DragFloat2(u8"ロード進捗位置", &_loadProgressPosition.x, 1.0f);
	ImGui::DragFloat2(u8"ロード進捗スケール", &_loadProgressScale.x, 0.1f);
}

// 3D描画後の描画処理
void QuestUIController::DelayedRender(const RenderContext& rc)
{
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;
	auto questController = _questController.lock();
	if (!questController)
		return;
	auto currentQuest = questController->GetCurrentQuestData();
	if (!currentQuest)
		return;

	Vector2 sprWorldPos = spriteRenderer->GetRectTransform(Front).GetWorldPosition();

	TextRenderer& textRenderer = GetActor()->GetScene()->GetTextRenderer();
	// クエスト名描画
	textRenderer.Draw(
		FontType::MSGothic,
		ToUtf16(currentQuest->name).c_str(),
		sprWorldPos + _titlePosition,
		_titleColor,
		0.0f,
		Vector2::XOneYZero,
		_titleScale);

	// 受注中の処理
	if (questController->GetCurrentState() == QuestController::State::Accepted)
	{
		if (questController->CanStartQuest())
		{
			textRenderer.Draw(
				FontType::MSGothic,
				L"ロード完了",
				sprWorldPos + _loadProgressPosition,
				Vector4::Blue,
				0.0f,
				Vector2::XOneYZero,
				_loadProgressScale);
		}
		else
		{
			textRenderer.Draw(
				FontType::MSGothic,
				L"ロード中",
				sprWorldPos + _loadProgressPosition,
				Vector4::Red,
				0.0f,
				Vector2::XOneYZero,
				_loadProgressScale);
		}
	}
}
