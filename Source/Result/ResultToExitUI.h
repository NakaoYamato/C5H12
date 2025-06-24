#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class ResultToExitUI : public SpriteRenderer
{
public:
	ResultToExitUI() {}
	~ResultToExitUI() override = default;
	// 名前取得
	const char* GetName() const override { return "ResultToExitUI"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
};