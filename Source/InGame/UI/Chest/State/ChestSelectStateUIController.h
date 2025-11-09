#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class ChestSelectStateUIController : public Component
{
public:
	enum SelectMenuOption
	{
		ItemOption,
		ArmorOption,
		TestOption,
		MaxOption,
    };

public:
	ChestSelectStateUIController() {}
	~ChestSelectStateUIController() override {}
	// 名前取得
	const char* GetName() const override { return "ChestSelectStateUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

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

	int _index = 0;

	const float BackPositionInterval = 102.0f;
};