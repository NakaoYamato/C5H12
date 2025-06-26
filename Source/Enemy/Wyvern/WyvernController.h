#pragma once

#include "../../Library/Component/Component.h"
#include "StateMachine/WyvernStateMachine.h"

class WyvernController : public Component
{
public:
	WyvernController() {}
	~WyvernController() override {}
	// 名前取得
	const char* GetName() const override { return "WyvernController"; }
	// 開始処理
	void Start() override;
	// 遅延更新処理
	void LateUpdate(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
};