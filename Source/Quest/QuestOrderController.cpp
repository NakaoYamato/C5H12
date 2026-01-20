#include "QuestOrderController.h"

#include <Mygui.h>

// 開始処理
void QuestOrderController::Start()
{
	// ユーザーデータマネージャー取得
	_userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");
	// クエストデータマネージャー取得
	_questManager = ResourceManager::Instance().GetResourceAs<QuestManager>("QuestManager");
}

// 更新処理
void QuestOrderController::Update(float elapsedTime)
{
}

// Gui描画
void QuestOrderController::DrawGui()
{
	ImGui::Text(u8"受注中: %s", _isInQuest ? u8"はい" : u8"いいえ");
	if (ImGui::Button(u8"クエスト受注"))
	{
		AcceptQuest(0); // 仮にインデックス0のクエストを受注
	}
	if (ImGui::Button(u8"クエスト開始"))
	{
		StartQuest();
	}
	if (ImGui::Button(u8"クエスト終了(クリア)"))
	{
		EndQuest(true, 120.0f); // 仮にクリアして120秒で終了
	}
	if (ImGui::Button(u8"クエストキャンセル"))
	{
		CancelQuest();
	}
}

// クエスト受注
void QuestOrderController::AcceptQuest(int questIndex)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// クエスト選択処理呼び出し
	userDataManager->AcceptQuest(questIndex);

	_isInQuest = true;
}

// クエスト開始
void QuestOrderController::StartQuest()
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;
	// クエスト開始処理呼び出し
	userDataManager->StartQuest();
}

// 受注中のクエスト終了
void QuestOrderController::EndQuest(bool clear, float time)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;
	// クエスト終了処理呼び出し
	userDataManager->EndQuest(clear, time);
}

// クエストキャンセル
void QuestOrderController::CancelQuest()
{
	_isInQuest = false;
}
