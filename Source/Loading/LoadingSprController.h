#pragma once

#include "../../Library/Component/SpriteRenderer.h"
#include "../../Library/Actor/UI/UIActor.h"

#include "../../Scene/SceneLoading.h"

class LoadingSprController : public Component
{
public:
	LoadingSprController() {}
	~LoadingSprController() override {}
	// 名前取得
	const char* GetName() const override { return "LoadingSprController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
	// 各種スプライトの名前
	const std::string BackSpr = "Back";
	const std::string LoadingBarBackSpr = "LoadingBarBack";
	const std::string LoadingBarSpr = "LoadingBar";
	const std::string TextSprs[11] = 
	{ 
		"N0",
		"O0",
		"W",
		" ",
		"L",
		"O1",
		"A",
		"D",
		"I",
		"N1",
		"G",
	};

	// ロードバーの幅
	float _loadingBarWidth = 0.0f;
	// ロードバーの進捗速度
	float _loadingBarSpeed = 3.0f;

	float _timer = 0.0f;
	float _intervalTimer = 0.0f;

	Vector2 _textStartPos = Vector2(1100.0f, 900.0f);
	float _textInterval = 55.0f;
	float _textMoveTime = 0.45f;
	float _textMoveInterval = 0.1f;
	Vector2 _textMoveOffset = Vector2(30.0f, 90.0f);
	float _textMoveDelay = 0.55f;

	SceneLoading* _loadingScene = nullptr;
};
