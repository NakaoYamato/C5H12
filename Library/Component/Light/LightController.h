#pragma once

#include "../Component.h"

class LightController : public Component
{
public:
	LightController() {}
	~LightController()override {}

	// 名前取得
	const char* GetName()const { return "LightController"; }

	// デバッグ表示
	void DebugRender(const RenderContext& rc) override;

	// GUI描画
	void DrawGui() override;

#pragma region アクセサ
	Vector4 GetDirection() { return GetActor()->GetTransform().GetAxisZ(); }
	const Vector4& GetColor()const { return _color; }
	const Vector4& GetAmbientColor()const { return _ambientColor; }
#pragma endregion
private:
	Vector4 _color = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 _ambientColor = { 1.0f,1.0f,1.0f,1.0f };

	// デバッグ用
	int _currentIndex = 0;
	// 光の始点
	Vector3 _lightStart = {};
	// 光の終点
	Vector3 _lightEnd = {};
};