#pragma once

#include "../../Library/Component/SpriteRenderer.h"

#include "Menu/ChestSelectMenuController.h"
#include "Menu/ChestArmorMenuController.h"

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

	// インデックス追加
	void AddIndex(int val);
	// 次へ進む
	void NextState();
	// 前の状態へ戻る
	void PreviousState();

	// チェストUIを開く
	void Open();

	// チェストUIを閉じる
	void Close();

	// 使用中のチェストアクター設定
	void SetChestActor(const std::shared_ptr<Actor>& actor) { _chestActor = actor; }
private:
	void UpdateToArmorMenu(float elapsedTime);
	void UpdateFromArmorMenu(float elapsedTime);

private:
	std::weak_ptr<ChestSelectMenuController> _selectUI;
	std::weak_ptr<ChestArmorMenuController> _armorUI;

	std::weak_ptr<ChangeArmorCamera> _changeArmorCamera;

	// メニュー状態
	State _state = State::SelectMenu;

	float _fadeTime = 0.4f;

	// 使用中のチェストアクター
	std::weak_ptr<Actor> _chestActor;
};
