#pragma once
#include "Component.h"

// シェーダー側で定数バッファーに入れる構造体
// MaxObjectConstant以上のオブジェクトを考慮していない
class ObstacleController : public Component
{
public:
	~ObstacleController() override = default;
	// 名前取得
	const char* GetName() const override { return "ObstacleController"; }
	// 遅延更新処理
	void LateUpdate(float elapsedTime) override;
	// デバッグ表示
	void DebugRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;
private:
	// オフセット
	Vector3 _offset{};
	// 半径
	float _radius = 0.5f;
};