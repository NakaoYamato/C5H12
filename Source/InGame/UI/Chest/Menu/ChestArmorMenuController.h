#pragma once

#include "../../Library/Component/SpriteRenderer.h"

#include "../../Source/Player/PlayerArmorController.h"

class ChestArmorMenuController : public Component
{
public:
	enum class SelectType
	{
		Weapon,

		Head,
		Chest,
		Arm,
		Waist,
		Leg,

		MaxType,
	};

	enum class State
	{
		SelectArmorType,
		SelectArmor,
		MaxState,
	};

public:
	ChestArmorMenuController() {}
	~ChestArmorMenuController() override {}
	// 名前取得
	const char* GetName() const override { return "ChestArmorMenuController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

	// インデックス追加
	void AddIndex(int val);
	// 装備インデックス追加
	void AddSelectArmorColumnIndex(int val);
	// 装備インデックス追加
	void AddSelectArmorRowIndex(int val);

	// 次へ進む
	void NextState();
	// 前の状態へ戻る
	// メニューを閉じる場合はtrueを返す
	bool PreviousState();
	// リセット
	void Reset();

	void SetPlayerArmorController(const std::shared_ptr<PlayerArmorController>& controller);

	SelectType GetSelectType() const { return _selectType; }
	State GetState() const { return _state; }
private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
	// プレイヤー防具コントローラー
	std::weak_ptr<PlayerArmorController> _playerArmorController;

	// 各種スプライトの名前
	const std::string BackSpr = "Back";
	const std::string WeaponBackSpr = "WeaponBack";
	const std::string HeadBackSpr = "HeadBack";
	const std::string ChestBackSpr = "ChestBack";
	const std::string ArmBackSpr = "ArmBack";
	const std::string WaistBackSpr = "WaistBack";
	const std::string LegBackSpr = "LegBack";

	const std::string WeaponSpr = "Weapon";
	const std::string HeadSpr = "Head";
	const std::string ChestSpr = "Chest";
	const std::string ArmSpr = "Arm";
	const std::string WaistSpr = "Waist";
	const std::string LegSpr = "Leg";

	const std::string FrontSpr = "Front";
	const std::string BoxBackSpr = "BoxBack";

	// テキストパラメータ
	Vector2 _textOffset = Vector2(110.0f, -20.0f);
	Vector2 _textOrigin = Vector2::Zero;
	Vector2 _textScale = Vector2::One;
	Vector4 _textColor = Vector4::White;

	SelectType _selectType = SelectType::Weapon;
	State _state = State::SelectArmorType;

	const Vector2 SprStartPos = Vector2(160.0f, 180.0f);
	const float BackPositionInterval = 120.0f;

	std::unique_ptr<Sprite> _armorBackSprite;
	std::unique_ptr<Sprite> _armorSprites[static_cast<int>(SelectType::MaxType)];
	std::unique_ptr<Sprite> _armorFrontSprite;

	Vector2 _armorSprPositionInterval = Vector2(140.0f, 140.0f);
	int _armorSprColumnNum = 5;
	int _armorSprRowNum = 5;

	int _selectArmorColumnIndex = 0;
	int _selectArmorRowIndex = 0;
};