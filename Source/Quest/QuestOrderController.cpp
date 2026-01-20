#include "QuestOrderController.h"

#include <Mygui.h>

// 開始処理
void QuestOrderController::Start()
{
	// ユーザーデータマネージャー取得
	_userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");
	// クエストデータマネージャー取得
	_questManager = ResourceManager::Instance().GetResourceAs<QuestManager>("QuestManager");
	// 敵データマネージャー取得
	_enemyDataManager = ResourceManager::Instance().GetResourceAs<EnemyDataManager>("EnemyDataManager");
}

// 更新処理
void QuestOrderController::Update(float elapsedTime)
{
}

// Gui描画
void QuestOrderController::DrawGui()
{
	ImGui::Text(u8"受注中: %s", _isInQuest ? u8"はい" : u8"いいえ");
	ImGui::Text(u8"クエスト開始可能: %s", CanStartQuest() ? u8"はい" : u8"いいえ");

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
	auto questManager = _questManager.lock();
	if (!questManager)
		return;
	auto enemyDataManager = _enemyDataManager.lock();
	if (!enemyDataManager)
		return;

	// クエストインデックス保存
	_questIndex = questIndex;
	// 受注中フラグON
	_isInQuest = true;

	_targetNameList.clear();
	_currentQuestData = questManager->GetQuestData(static_cast<size_t>(questIndex));
	if (_currentQuestData)
	{
		// ターゲットのモデルを読み込み開始
		for (const auto& target : _currentQuestData->targets)
		{
			enemyDataManager->LoadModel(target.name);
			_targetNameList.push_back(target.name);
		}
	}
}

// クエスト開始
void QuestOrderController::StartQuest()
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// 受注回数増加
	userDataManager->IncreaseQuestOrderCount(_questIndex);
}

// 受注中のクエスト終了
void QuestOrderController::EndQuest(bool clear, float time)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	if (clear)
	{
		userDataManager->IncreaseQuestClearCount(_questIndex);
		userDataManager->UpdateQuestBestClearTime(_questIndex, time);
	}
}

// クエストキャンセル
void QuestOrderController::CancelQuest()
{
	_isInQuest = false;
}

// クエスト開始可能か
bool QuestOrderController::CanStartQuest() const
{
	// 受注中のクエストがない場合は開始不可
	if (!_isInQuest)
		return false;

	// クエストデータがない場合は開始不可
	if (!_currentQuestData)
		return false;

	// ターゲットのモデルがすべて読み込まれていない場合は開始不可
	auto enemyDataManager = _enemyDataManager.lock();
	if (!enemyDataManager)
		return false;
	for (const auto& targetName : _targetNameList)
	{
		if (!enemyDataManager->IsModelLoaded(targetName))
			return false;
	}

	return true;
}
