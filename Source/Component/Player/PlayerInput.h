#pragma once

#include "../../Library/Component/Component.h"

class PlayerInput : public Component
{
public:
	enum Inputs : int
	{
		Attack		= 0b00000001,
		Dash		= 0b00000010,
		Evade		= 0b00000100,
		Guard		= 0b00001000,
	};

public:
	PlayerInput() {}
	~PlayerInput() override {}

	// 名前取得
	const char* GetName() const override { return "PlayerInput"; }

	// 開始処理
	void Start() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// GUI描画
	void DrawGui() override;

#pragma region アクセサ
	// 入力方向を取得
	const Vector2& GetLAxisValue() const { return _lAxisValue; }
	// 移動方向を取得
	const Vector3& GetMovement() const { return _movement; }
	int GetInputFlag() const { return _inputFlag; }
#pragma endregion
private:
	// 入力方向
	Vector2 _lAxisValue = { 0.0f, 0.0f };
	// 移動方向(ワールド座標)
	Vector3 _movement;

	// フラグ
	int _inputFlag = 0;
};