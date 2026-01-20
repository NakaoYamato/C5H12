#include "QuestOrderController.h"

#include "../../Library/Scene/Scene.h"

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

	// メタAI取得
	_metaAI = GetActor()->GetComponent<MetaAI>();
}

// 更新処理
void QuestOrderController::Update(float elapsedTime)
{
	auto metaAI = _metaAI.lock();
	if (!metaAI)
		return;
	auto enemyDataManager = _enemyDataManager.lock();
	if (!enemyDataManager)
		return;

	if (_startQuestFlag)
	{
		// フェードが完了したらプレイヤーを初期位置に移動、ターゲットの生成
		if (!GetActor()->GetScene()->GetFade()->IsFading())
		{
			if (auto playerActor = GetActor()->GetScene()->GetActorManager().FindByClass<PlayerActor>(ActorTag::Player))
			{
				if (auto playerController = playerActor->GetComponent<PlayerController>())
				{
					// メタAIからリスポーン位置を取得
					EntryZone* entryZone = _metaAI.lock()->SearchNearestEntryZone(Targetable::Faction::Player, GetActor()->GetTransform().GetWorldPosition());
					if (entryZone)
					{
						Vector3 respawnPosition = entryZone->GetActor()->GetTransform().GetWorldPosition() + entryZone->GetCenter();
						Vector3 respawnAngle = entryZone->GetAngle();
						playerController->Respawn(respawnPosition, respawnAngle);
					}
				}
			}

			int index = 0;
			for (const auto& targetName : _targetNameList)
			{
				std::string name = "Target" + std::to_string(index++);
				enemyDataManager->CreateActor(GetActor()->GetScene(), targetName, name);
			}

			_startQuestFlag = false;
			// フェードイン開始
			GetActor()->GetScene()->GetFade()->Start(Fade::Type::FadeIn, 1.0f);
		}
	}
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
	// すでに受注中の場合は処理しない
	if (_currentState != State::Idle)
		return;

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

	// 状態を受注中に変更
	_currentState = State::Accepted;
}

// クエスト開始
void QuestOrderController::StartQuest()
{
	// 受注中以外の場合は処理しない
	if (_currentState != State::Accepted)
		return;

	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// 受注回数増加
	userDataManager->IncreaseQuestOrderCount(_questIndex);

	_startQuestFlag = true;
	// 状態をクエスト中に変更
	_currentState = State::InQuest;

	// フェードアウト開始
	GetActor()->GetScene()->GetFade()->Start(Fade::Type::FadeOut, 1.0f);
}

// 受注中のクエスト終了
void QuestOrderController::EndQuest(bool clear, float time)
{
	// クエスト中以外の場合は処理しない
	if (_currentState != State::InQuest)
		return;

	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	if (clear)
	{
		userDataManager->IncreaseQuestClearCount(_questIndex);
		userDataManager->UpdateQuestBestClearTime(_questIndex, time);
	}

	// 状態を完了に変更
	_currentState = State::Completed;
}

// クエストキャンセル
void QuestOrderController::CancelQuest()
{
	// 受注中以外の場合は処理しない
	if (_currentState != State::Accepted)
		return;

	_isInQuest = false;

	// 状態を待機に変更
	_currentState = State::Idle;
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
