#pragma once

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Component/SpriteRenderer.h"

class PlayerShapnessUIController : public Component
{
public:
	PlayerShapnessUIController() = default;
	~PlayerShapnessUIController() override = default;
	// 名前取得
	const char* GetName() const override { return "PlayerShapnessUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
};