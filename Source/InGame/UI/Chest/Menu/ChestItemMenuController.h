#pragma once

#include "../../Library/Component/SpriteRenderer.h"

#include "../../Source/User/UserDataManager.h"
#include "../../Library/Scene/Scene.h"

class ChestItemMenuController : public Component
{
public:
	enum class State
	{
		Pourch,
		Strage,
		MaxState,
	};

public:
	ChestItemMenuController() {}
	~ChestItemMenuController() override {}
	// 名前取得
	const char* GetName() const override { return "ChestItemMenuController"; }

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
	// 次へ進む
	void NextState();
	// 前の状態へ戻る
	// メニューを閉じる場合はtrueを返す
	bool PreviousState();
	// リセット
	void Reset();

	State GetState() const { return _state; }
private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
	// ユーザーデータマネージャー
	std::weak_ptr<UserDataManager> _userDataManager;

	// 各種スプライトの名前
	const std::string BackSpr		= "Back";
	const std::string PourchBackSpr = "PourchBack";
	const std::string StrageBackSpr = "StrageBack";
	const std::string FrontSpr		= "Front";
	const std::string BoxBackSpr	= "BoxBack";
	const std::string TextBoxSpr	= "TextBox";

	State _state = State::Pourch;
};