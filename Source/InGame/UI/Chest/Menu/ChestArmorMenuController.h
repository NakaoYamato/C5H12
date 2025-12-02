#pragma once

#include "ChestMenuControllerBase.h"

#include "../../Source/Player/PlayerEquipmentController.h"
#include "../../Library/Scene/Scene.h"

class ChestArmorMenuController : public ChestMenuControllerBase
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
	void AddIndex(int direction) override;
	// 装備インデックス追加
	void AddSelectArmorColumnIndex(int val);
	// 装備インデックス追加
	void AddSelectArmorRowIndex(int val);

	// 次へ進む
	void NextState() override;
	// 前の状態へ戻る
	// メニューを閉じる場合はtrueを返す
	bool PreviousState() override;
	// リセット
	void Reset() override;

	void SetPlayerEquipmentController(const std::shared_ptr<PlayerEquipmentController>& controller);

	SelectType GetSelectType() const { return _selectType; }
	State GetState() const { return _state; }
private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
	// プレイヤー装備コントローラー
	std::weak_ptr<PlayerEquipmentController> _PlayerEquipmentController;
	// ユーザーデータマネージャー
	std::weak_ptr<UserDataManager> _userDataManager;

	// 各種スプライトの名前
	const std::string BackSpr = "Back";
	const std::string WeaponBackSpr = "WeaponBack";
	const std::string HeadBackSpr = "HeadBack";
	const std::string ChestBackSpr = "ChestBack";
	const std::string ArmBackSpr = "ArmBack";
	const std::string WaistBackSpr = "WaistBack";
	const std::string LegBackSpr = "LegBack";

	const std::string WeaponSprs[static_cast<int>(WeaponType::WeaponTypeMax)] =
	{
		"GreatSword",
	};
	const std::string HeadSpr = "Head";
	const std::string ChestSpr = "Chest";
	const std::string ArmSpr = "Arm";
	const std::string WaistSpr = "Waist";
	const std::string LegSpr = "Leg";

	const std::string FrontSpr = "Front";
	const std::string BoxBackSpr = "BoxBack";

	const std::string TextBoxSpr = "TextBox";

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
	std::unique_ptr<Sprite> _weaponSprites[static_cast<int>(WeaponType::WeaponTypeMax)];
	std::unique_ptr<Sprite> _armorSprites[static_cast<int>(ArmorType::Leg) + 1];
	std::unique_ptr<Sprite> _armorFrontSprite;

	Vector2 _armorSprPositionInterval = Vector2(140.0f, 140.0f);
	int _armorSprColumnNum = 4;
	int _armorSprRowNum = 5;

	int _selectArmorColumnIndex = 0;
	int _selectArmorRowIndex = 0;

	// 防具説明テキストパラメータ
	TextRenderer::TextDrawData _nameTextData;
	TextRenderer::TextDrawData _rarityextData;
	TextRenderer::TextDrawData _defenseTextData;
	TextRenderer::TextDrawData _skillTextData;
	float _skillTextIntervalY = 45.0f;

	// 選択中のリムライト色
	Vector4 _selectedRimLightColor = Vector4(1.0f, 1.0f, 0.0f, 0.6f);
};