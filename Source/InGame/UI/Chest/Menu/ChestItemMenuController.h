#pragma once

#include "../../Library/Component/SpriteRenderer.h"

#include "../../Source/User/UserDataManager.h"
#include "../../Library/Scene/Scene.h"

class ChestItemMenuController : public Component
{
public:
	enum class Tab
	{
		Pourch,
		Strage,
	};

	enum class State
	{
		ItmeSelect,

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

	// タブ切り替え
	void ChangeTab();

	Tab GetTab() const { return _tab; }
	State GetState() const { return _state; }
	int GetCurrentIndex() const { return _currentIndex; }

	int GetStrageItemColumnIndex() const { return _strageItemColumnIndex; }
	int GetStrageItemRowIndex() const { return _strageItemRowIndex; }

#pragma region 入出力
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;
#pragma endregion

private:
	// ポーチメニュー描画
	void RenderPourch(const RenderContext& rc);

	// ストレージメニュー描画
	void RenderStrage(const RenderContext& rc);

private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
	// ユーザーデータマネージャー
	std::weak_ptr<UserDataManager> _userDataManager;

	// 各種スプライトの名前
	const std::string PourchBackSpr = "PourchBack";
	const std::string StrageBackSpr = "StrageBack";
	const std::string TextBoxSpr	= "TextBox";

	Tab _tab = Tab::Pourch;
	State _state = State::ItmeSelect;
	int _currentIndex = 0;

	// ポーチ内アイテム用トランスフォーム
	RectTransform _pourchItemTransform;
	Vector2 _pourchItemOffset = Vector2(90.0f, 0.0f);
	Vector2 _pourchItemScale = Vector2(2.0f, 2.0f);
	// ストレージ内アイテム用トランスフォーム
	RectTransform _strageItemTransform;
	Vector2 _strageItemOffset = Vector2(90.0f, 90.0f);
	Vector2 _strageItemScale = Vector2(2.0f, 2.0f);
	int _strageItemColumnIndex = 5;
	int _strageItemRowIndex = 10;
	int _strageItemMaxPage = 3;

	// アイテムの背景スプライト
	std::unique_ptr<Sprite> _itemBackSprite;
	// アイテムの前面スプライト
	std::unique_ptr<Sprite> _itemFrontSprite;

	// 個数表示用パラメータ
	Vector2 _itemQuantityOffset = Vector2(30.0f, 30.0f);
	Vector2 _itemQuantityScale = Vector2(1.0f, 1.0f);

	// アイテムの名前
	TextRenderer::TextDrawData _itemName;
	// アイテムの説明
	TextRenderer::TextDrawData _itemDescription;
};