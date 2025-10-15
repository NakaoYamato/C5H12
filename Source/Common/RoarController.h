#pragma once

#include "../../Library/Component/Component.h"

class RoarController : public Component
{
public:
	RoarController() {}
	~RoarController() override {}
	// 名前取得
	const char* GetName() const override { return "RoarController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	void SetWorldPosition(const Vector3& position);
	void SetRadius(float radius) { _radius = radius; }
	void SetRate(float rate) { _rate = rate; }

private:
	Vector3 _worldPosition = Vector3::Zero;
	Vector2 _screenPosition = Vector2::Zero;
	float _radius = 300.0f;
	float _rate = 0.0f;
};