#pragma once

#include "../../Library/Component/SpriteRenderer.h"

#include "Menu/ChestSelectMenuController.h"
#include "Menu/ChestArmorMenuController.h"
#include "Menu/ChestItemMenuController.h"

#include "../../Source/Camera/ChangeArmorCamera.h"

class ChestUIController : public Component
{
public:
	enum State
	{
		SelectMenu,
		ItemMenu,
		ArmorMenu,

		ToArmorMenu,
		FromArmorMenu,
	};

	enum class InputDirection : unsigned int
	{
		None	= 0b0000,
		Up		= 0b0001,
		Down	= 0b0010,
		Left	= 0b0100,
		Right	= 0b1000,
	};
	enum class InputState
	{
		None,

		Select,
		Back,
		L3,
		R3,
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

	// GUI描画
	void DrawGui() override;

	// ステート初期化
	void InitState();

	// チェストUIを開く
	void Open();

	// チェストUIを閉じる
	void Close();

	// 使用中のチェストアクター設定
	void SetChestActor(const std::shared_ptr<Actor>& actor) { _chestActor = actor; }
	// 入力値設定
	void SetInputState(InputState inputState) { _inputState = inputState; }
	// 入力値設定
	void SetInputDirection(int inputDirection) { _inputDirection = inputDirection; }
private:
	// Selectメニューの入力処理
	void SelectInput();
	// Armorメニューの入力処理
	void ArmorMenuInput();
	// Itemメニューの入力処理
	void ItemMenuInput();

	void UpdateToArmorMenu(float elapsedTime);
	void UpdateFromArmorMenu(float elapsedTime);

private:
	std::weak_ptr<ChestSelectMenuController> _selectUI;
	std::weak_ptr<ChestArmorMenuController> _armorUI;
	std::weak_ptr<ChestItemMenuController> _itemUI;

	ChangeArmorCamera* _changeArmorCamera;

	// メニュー状態
	State _state = State::SelectMenu;
	// 入力値
	InputState _inputState = InputState::None;
	int _inputDirection = 0;

	float _fadeTime = 0.4f;

	// 使用中のチェストアクター
	std::weak_ptr<Actor> _chestActor;
	// プレイヤーアクター
	std::weak_ptr<Actor> _playerActor;
};
