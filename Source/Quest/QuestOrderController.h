#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/User/UserDataManager.h"

#include "../../Source/AI/MetaAI.h"

#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Player/PlayerController.h"

class QuestOrderController : public Component
{
public:
	enum class State
	{
		Idle,
		Accepted,
		InQuest,
		Completed,
	};

public:
	QuestOrderController() = default;
	~QuestOrderController() override {}

	// 名前取得
	const char* GetName() const override { return "QuestOrderController"; }

	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// Gui描画
	void DrawGui() override;

	// クエスト受注
	void AcceptQuest(int questIndex);
	// クエスト開始
	void StartQuest();
	// 受注中のクエスト終了
	void EndQuest(bool clear, float time);
	// クエストキャンセル
	void CancelQuest();
	// クエスト開始可能か
	bool CanStartQuest() const;
	// クエスト中か
	bool IsInQuest() const { return _isInQuest; }
	// 現在の状態取得
	State GetCurrentState() const { return _currentState; }

private:
	// ユーザーデータマネージャー
	std::weak_ptr<UserDataManager> _userDataManager;
	// クエストデータマネージャー
	std::weak_ptr<QuestManager> _questManager;
	// 敵情報
	std::weak_ptr<EnemyDataManager> _enemyDataManager;
	// メタAI
	std::weak_ptr<MetaAI> _metaAI;

	// 現在の状態
	State _currentState = State::Idle;

	// 現在受注中のクエストデータ
	QuestData* _currentQuestData = nullptr;
	// ターゲットの名前一覧
	std::vector<std::string> _targetNameList;
	// クエストインデックス
	int _questIndex = -1;
	bool _isInQuest = false;

	// クエスト開始時用パラメータ
	bool _startQuestFlag = false;
	float _timer = 0.0f;
};