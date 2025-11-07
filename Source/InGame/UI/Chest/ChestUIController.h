#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class ChestUIController : public Component
{
public:
	enum State
	{
		SelectMenu,
		ItemMenu,
		ArmorMenu,
	};

	enum SelectMenuOption
	{
		ItemOption,
		ArmorOption,
		TestOption,

		MaxOption,
	};

public:
	ChestUIController() {}
	~ChestUIController() override {}
	// 名前取得
	const char* GetName() const override { return "ChestUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;

	// GUI描画
	void DrawGui() override;

	// ステート初期化
	void InitState();

	// index加算
	void AddIndex(int val) { _index += val; }
	// 次へ進む
	void NextState();
	// 前の状態へ戻る
	void PreviousState();

private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;

	// 各種スプライトの名前
	const std::string ItemBackSpr = "ItemBack";
	const std::string ArmorBackSpr = "ArmorBack";
	const std::string TestBackSpr = "TestBack";
	const std::string FrontSpr = "Front";

	// テキストパラメータ
	Vector2 _textOffset = Vector2(-200.0f, -20.0f);
	Vector2 _textOrigin = Vector2::Zero;
	Vector2 _textScale = Vector2::One;

	// メニュー状態
	State _state = State::SelectMenu;

	int _index = 0;

	const float BackPositionInterval = 102.0f;
};
