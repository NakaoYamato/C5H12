#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class ChestArmorMenuController : public Component
{
public:
	enum class AromrTypeSelectState
	{
		Weapon,

		Head,
		Chest,
		Arm,
		Waist,
		Leg,

		MaxType,
	};

public:
	ChestArmorMenuController() {}
	~ChestArmorMenuController() override {}
	// 名前取得
	const char* GetName() const override { return "ChestArmorMenuController"; }
	// 開始処理
	void Start() override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

	// インデックス追加
	void AddIndex(int val);
	// リセット
	void ResetIndex() { _index = AromrTypeSelectState::Weapon; }

	AromrTypeSelectState GetSelectIndex() const { return _index; }

private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;

	// 各種スプライトの名前
	const std::string WeaponBackSpr = "WeaponBack";
	const std::string HeadBackSpr = "HeadBack";
	const std::string ChestBackSpr = "ChestBack";
	const std::string ArmBackSpr = "ArmBack";
	const std::string WaistBackSpr = "WaistBack";
	const std::string LegBackSpr = "LegBack";
	const std::string FrontSpr = "Front";

	// テキストパラメータ
	Vector2 _textOffset = Vector2(110.0f, -20.0f);
	Vector2 _textOrigin = Vector2::Zero;
	Vector2 _textScale = Vector2::One;
	Vector4 _textColor = Vector4::White;

	AromrTypeSelectState _index = AromrTypeSelectState::Weapon;

	const Vector2 SprStartPos = Vector2(160.0f, 180.0f);
	const float BackPositionInterval = 120.0f;
};