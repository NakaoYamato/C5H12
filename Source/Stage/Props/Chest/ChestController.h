#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/InGame/Input/ChestInput.h"
#include "../../Source/Common/InteractionController.h"

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

private:
	std::weak_ptr<Actor> _topActor;
	std::weak_ptr<ChestInput> _chestInput;
	std::weak_ptr<InteractionController> _interactionController;

	float _angleSpeed = DirectX::XMConvertToRadians(90.0f);
	float _openAngleX = DirectX::XMConvertToRadians(50.0f);

	// 選択UIのオフセット
	Vector3 _selectUIOffset = Vector3(0.0f, 1.5f, 0.0f);
};