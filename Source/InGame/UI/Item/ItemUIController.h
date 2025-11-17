#pragma once

#include "../../Library/Component/SpriteRenderer.h"
#include "../../Source/User/UserDataManager.h"

class ItemUIController : public Component
{
public:
	enum State
	{
		Closed,
		Opening,
		Opened,
		Closing,

		StateMax
	};
public:
	ItemUIController() {}
	~ItemUIController() override {}
	// 名前取得
	const char* GetName() const override { return "ItemUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

	// 開く
	void Open();
	// 閉じる
	void Close();

	bool IsClosed() const { return _state == State::Closed; }
	bool IsOpen() const { return _state == State::Opened; }

	void SetCurrentIndex(int index) { _currentIndex = index; }
private:
	RectTransform* _myRectTransform = nullptr;
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
	std::weak_ptr<SpriteRenderer> _sliderSpriteRenderer;

	// 各種スプライトの名前
	const std::string BackSpr = "Back";

	std::weak_ptr<UserDataManager> _userDataManager;
	int _currentIndex = 0;

	// 閉じている時のパラメータ
	Vector2 _closePosition = Vector2(1800.0f, 940.0f);
	Vector2 _closeScale = Vector2(0.8f, 0.8f);
	Vector2 _closeItemPosition = Vector2(0.0f, 38.0f);
	// 開いている時のパラメータ
	Vector2 _openPosition = Vector2(1700.0f, 930.0f);
	Vector2 _openScale = Vector2(1.0f, 1.0f);
	Vector2 _openItemPosition = Vector2(0.0f, 45.0f);
	float _lerpSpeed = 15.0f;

	// アイコン用トランスフォーム
	RectTransform _iconTransform;
	Vector2 _iconOffset = Vector2(90.0f, 2.0f);
	Vector2 _iconScaleOffset = Vector2(0.7f, 0.7f);

	// 所持数描画用トランスフォーム
	RectTransform _quantityTransform;
	Vector4 _quantityColor = Vector4::Black;
	Vector4 _quantityMaxColor = Vector4(1.0f, 70.0f / 255.0f, 0.0f, 1.0f);

	// アイテム名
	Vector2 _itemNameOffset = Vector2(0.0f, 35.0f);
	Vector2 _itemNameOrigin = Vector2(0.5f, 0.5f);
	Vector2 _itemNameScaleOffset = Vector2(0.7f, 0.7f);

	State _state = State::Closed;
	float _stateTimer = 0.0f;
};