#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/InGame/Input/ChestInput.h"

class ChestController : public Component
{
public:
	ChestController() = default;
	~ChestController() override {}
	// 名前取得
	const char* GetName() const override { return "ChestController"; }
	// 初期化処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画処理
	void DrawGui() override;

	// 開く処理
	bool Open();
private:
	std::weak_ptr<Actor> _topActor;
	std::weak_ptr<ChestInput> _chestInput;

	float _angleSpeed = DirectX::XMConvertToRadians(90.0f);
	float _openAngleX = DirectX::XMConvertToRadians(50.0f);
};