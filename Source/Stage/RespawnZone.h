#pragma once

#include "../../Library/Component/Component.h"

class RespawnZone : public Component, public std::enable_shared_from_this<RespawnZone>
{
public:
	RespawnZone() {}
	~RespawnZone() override {}
	// 名前取得
	const char* GetName() const override { return "RespawnZone"; }
	// 開始処理
	void Start() override;
	// GUI描画
	void DrawGui() override;
	// デバッグ表示
	void DebugRender(const RenderContext& rc) override;

	// 中心位置取得・設定
	const Vector3& GetCenter() const { return _center; }
	void SetCenter(const Vector3& center) { _center = center; }
	// 向き取得・設定
	const Vector3& GetAngle() const { return _angle; }
	void SetAngle(const Vector3& angle) { _angle = angle; }
	// 半径取得・設定
	float GetRadius() const { return _radius; }
	void SetRadius(float radius) { _radius = radius; }
	// デバッグレンダリング有効・無効設定
	void SetDebugRenderEnabled(bool enabled) { _debugRender = enabled; }

private:
	// 中心位置
	Vector3 _center{};
	// 向き
	Vector3 _angle{};
	// 半径
	float _radius = 5.0f;
	// デバッグレンダリング有効フラグ
	bool _debugRender = true;
};