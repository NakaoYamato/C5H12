#pragma once

#include "../../Library/Component/Component.h"

class LocusRenderer : public Component
{
public:
	struct LocusPoint
	{
		Vector3 rootPosition{};
		Vector3 tipPosition{};
		Vector3 velocity{};
		float	lifeTime = 0.0f; // 追加した時刻からの経過時間
	};

public:
	LocusRenderer() {}
	~LocusRenderer() override {}
	// 名前取得
	const char* GetName() const override { return "LocusRenderer"; }
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// 描画処理
	void Render(const RenderContext& rc) override;

	// 軌跡追加
	void PushFrontVertex(const Vector3& rootWP, 
		const Vector3& tipWP, 
		const Vector3& velocity,
		float lifeTime);

	void SetRootColor(const Vector4& v) { this->_rootColor = v; }
	void SetTipColor(const Vector4& v) { this->_tipColor = v; }
	void ResetTrialPos() { _trailPositions.clear(); }
private:
	std::vector<LocusPoint> _trailPositions;
	Vector4					_rootColor = { 1, 1, 1, 0.5f };
	Vector4					_tipColor = { 1.0f, 0.8f, 0.8f, 0.5f };
    float					_catmullRom = 3.0f; // Catmull-Romスプラインのパラメータ
    bool					_splineInterpolation = true; // スプライン補間を使用するかどうか
};