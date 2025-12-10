#pragma once

#include "Component.h"

class EnvironmentController : public Component
{
public:
	EnvironmentController() = default;
	~EnvironmentController() override = default;

	// 名前取得
	const char* GetName() const override { return "EnvironmentController"; }
	// 生成時処理
	void OnCreate() override;
	// 開始処理
	void Start() override;
	// 遅延更新処理
	void LateUpdate(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

};