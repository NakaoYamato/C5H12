#pragma once

#include "../Component.h"
#include "../../Library/Light/Light.h"

class LightController : public Component
{
public:
	LightController() {}
	~LightController()override {}

	// 名前取得
	const char* GetName()const { return "LightController"; }

	// 描画の前処理
	void RenderPreprocess(RenderContext& rc) override;

	// デバッグ表示
	void DebugRender(const RenderContext& rc) override;

	// GUI描画
	void DrawGui() override;

	// アクセサ
	Light& GetLight() { return _light; }
private:
	Light _light;
};