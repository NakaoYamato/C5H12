#pragma once

#include "../../Library/Component/Component.h"

class PlayerInput : public Component
{
public:
	enum Inputs : int
	{
		Attack = 0b00000001,
		Dash = 0b00000010,
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
	const Vector3& GetMovement() const { return _movement; }
	int GetInputFlag() const { return _inputFlag; }
#pragma endregion
private:
	// 移動方向(ワールド座標)
	Vector3 _movement;

	// フラグ
	int _inputFlag = 0;
};