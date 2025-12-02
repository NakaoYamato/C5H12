#pragma once

#include "ChestMenuControllerBase.h"

#include "../../Source/User/UserDataManager.h"
#include "../../Library/Scene/Scene.h"

class ChestItemMenuController : public ChestMenuControllerBase
{
public:
	enum class Tab
	{
		Pourch,
		Strage,
	};
	enum class Property
	{
		MoveAll,
		MoveOne,
		Cancel,
	};

	enum class State
	{
		ItemSelecting,
		PropertySelecting,

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
	void AddIndex(int direction) override;
	// 次へ進む
	void NextState() override;
	// 前の状態へ戻る
	// メニューを閉じる場合はtrueを返す
	bool PreviousState() override;
	// リセット
	void Reset() override;

	// タブ切り替え
	void ChangeTab();
	// ポーチ整列
	void SortPourch();

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

	// アイテム移動処理
	void ItemMove(bool all);

private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
	// ユーザーデータマネージャー
	std::weak_ptr<UserDataManager> _userDataManager;

	// 各種スプライトの名前
	const std::string PourchBackSpr = "PourchBack";
	const std::string StrageBackSpr = "StrageBack";
	const std::string TextBoxSpr	= "TextBox";

	Tab _tab = Tab::Pourch;
	Property _property = Property::MoveAll;
	State _state = State::ItemSelecting;
	int _currentIndex = 0;
    int _currentPage = 0;

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
	// プロパティ表示用スプライト
	std::unique_ptr<Sprite> _propertySprite;

	// プロパティ表示用パラメータ
	Vector2 _propertyOffset = Vector2(30.0f, 30.0f);
	Vector2 _propertyScale = Vector2(1.0f, 1.0f);
	float _propertyInversPosisionX = 1200.0f;
	float _propertyTextInterval = 30.0f;
	Vector2 _propertyTextOrigin = Vector2(0.0f, 0.0f);
	Vector2 _propertyTextScale = Vector2(1.0f, 1.0f);
	Vector4 _propertyTextColor = Vector4::White;
	Vector4 _propertySelectTextColor = Vector4::Orange;

	// 個数表示用パラメータ
	Vector2 _itemQuantityOffset = Vector2(30.0f, 30.0f);
	Vector2 _itemQuantityScale = Vector2(1.0f, 1.0f);

	// アイテムの名前
	TextRenderer::TextDrawData _itemName;
	// アイテムの説明
	TextRenderer::TextDrawData _itemDescription;
	// アイテムの個数
	TextRenderer::TextDrawData _itemQuantityText;
	// ポーチ
	TextRenderer::TextDrawData _pourchText;
	// ストレージ
	TextRenderer::TextDrawData _strageText;

	// 入力UI
	InputUI::DrawInfo _selectInputUI;
	TextRenderer::TextDrawData _selectInputUIText;
	InputUI::DrawInfo _backInputUI;
	TextRenderer::TextDrawData _backInputUIText;
	InputUI::DrawInfo _tabChangeInputUI;
	TextRenderer::TextDrawData _tabChangeInputUIText;
	InputUI::DrawInfo _sortInputUI;
	TextRenderer::TextDrawData _sortInputUIText;
};