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
	};

public:
	LocusRenderer() {}
	~LocusRenderer() override {}
	// –¼‘OŽæ“¾
	const char* GetName() const override { return "LocusRenderer"; }
	// XVˆ—
	void Update(float elapsedTime) override;
	// GUI•`‰æ
	void DrawGui() override;
	// •`‰æˆ—
	void Render(const RenderContext& rc) override;

	// ‹OÕ’Ç‰Á
	void PushFrontVertex(const Vector3& rootWP, const Vector3& tipWP, const Vector3& velocity);

	void SetRootColor(const Vector4& v) { this->_rootColor = v; }
	void SetTipColor(const Vector4& v) { this->_tipColor = v; }
	void ResetTrialPos() { _polygonEndIndex = 0; }
private:
	static const int MAX_POLYGON = 32;
	//Vector3					trailPositions[2][MAX_POLYGON];
	LocusPoint				_trailPositions[MAX_POLYGON];
	Vector4					_rootColor = { 1, 1, 1, 0.5f };
	Vector4					_tipColor = { 1.0f, 0.8f, 0.8f, 0.5f };
	int						_polygonStartIndex = 0;
	int						_polygonEndIndex = 0;
};