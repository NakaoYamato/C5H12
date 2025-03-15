#pragma once

#include "../Component.h"
#include "../../Library/Light/PointLight.h"

class PointLightController : public Component
{
public:
	PointLightController() {}
	~PointLightController()override {}

	// 名前取得
	const char* GetName()const { return "PointLightController"; }

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画の前処理
	void RenderPreprocess(RenderContext& rc) override;

	// デバッグ表示
	void DebugRender(const RenderContext& rc) override;

	// GUI描画
	void DrawGui() override;

	// アクセサ
	PointLight& GetLight() { return _light; }
private:
	PointLight _light;
};