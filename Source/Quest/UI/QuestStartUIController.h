#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class QuestStartUIController : public Component
{
public:
	QuestStartUIController() {}
	~QuestStartUIController() override {}
	// 名前取得
	const char* GetName() const override { return "QuestStartUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
	// 各種スプライトの名前
	const std::string FrontSpr = "Front";
	Material* _sprMaterial = nullptr;
	enum State
	{
		Entry,
		Loop,
		Exit,
	};
	State _state = State::Entry;
	float _timer = 0.0f;

	Vector2 _entryScale = Vector2(1.2f, 1.2f);

	float _entryTime = 0.5f;
	float _LoopTime = 1.0f;
	float _exitTime = 2.0f;
};