#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class ChestSelectMenuController : public Component
{
public:
	enum class SelectMenuOption
	{
		ItemOption,
		ArmorOption,
		TestOption,
		MaxOption,
    };

public:
	ChestSelectMenuController() {}
	~ChestSelectMenuController() override {}
	// 名前取得
	const char* GetName() const override { return "ChestSelectStateUIController"; }
	// 開始処理
	void Start() override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

	// インデックス追加
	void AddIndex(int val);
	// リセット
	void ResetIndex();

	SelectMenuOption GetSelectIndex() const { return _index; }

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
	Vector4 _textColor = Vector4::White;
	Vector4 _textSelectColor = Vector4::Yellow;

	SelectMenuOption _index = SelectMenuOption::ItemOption;

	const Vector2 SprStartPos = Vector2(300.0f, 200.0f);
	const float BackPositionInterval = 102.0f;
};