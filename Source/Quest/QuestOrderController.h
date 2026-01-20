#pragma once

#include "../../Library/Component/Component.h"

class QuestOrderController : public Component
{
public:
	QuestOrderController() = default;
	~QuestOrderController() override {}

	// 名前取得
	const char* GetName() const override { return "QuestOrderController"; }

	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;

	// クエスト受注
	void AcceptQuest(int questIndex);
	// クエスト開始
	void StartQuest();
	// 受注中のクエスト終了
	void EndQuest(bool clear, float time);
	// クエスト中か
	bool IsInQuest() const { return _isInQuest; }

private:
	float _elapsedTime = 0.0f;
	bool _isInQuest = false;
};