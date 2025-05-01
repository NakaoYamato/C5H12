#pragma once

#include "../Component.h"
#include "../../Library/Graphics/Light.h"

#include <mutex>

class PointLightController : public Component
{
public:
	PointLightController() {}
	~PointLightController()override {}

	// 名前取得
	const char* GetName()const { return "PointLightController"; }

	// 更新処理
	void Update(float elapsedTime) override;

	// デバッグ表示
	void DebugRender(const RenderContext& rc) override;

	// GUI描画
	void DrawGui() override;

	// アクセサ
	PointLight& GetLight() { return _light; }
private:
	PointLight _light;

	Vector4 _color = _VECTOR4_WHITE;
	float _colorVolume = 1.0f;
	static std::mutex _renderContextMutex;
};