#include "LocusRenderer.h"

#include "../../Library/Scene/Scene.h"
#include <imgui.h>

// 更新処理
void LocusRenderer::Update(float elapsedTime)
{
	// 保存していた頂点バッファを１フレーム分ずらす
	for (int i = MAX_POLYGON - 1; i >= 1; --i)
	{
		_trailPositions[i].rootPosition = _trailPositions[i - 1].rootPosition;
		_trailPositions[i].tipPosition	= _trailPositions[i - 1].tipPosition;
		_trailPositions[i].velocity		= _trailPositions[i - 1].velocity;
		// 速度更新
		const Vector3 movement = _trailPositions[i].velocity * elapsedTime;
		_trailPositions[i].rootPosition += movement;
		_trailPositions[i].tipPosition += movement;

		// 軌跡の外側に広げる
		const Vector3 vec = Vector3::Normalize(_trailPositions[i].tipPosition - _trailPositions[i].rootPosition) * elapsedTime;
		_trailPositions[i].rootPosition += vec * 2.0f;
		_trailPositions[i].tipPosition += vec;
	}
	_polygonStartIndex = std::clamp(_polygonStartIndex + 1, 0, MAX_POLYGON);
}
// GUI描画
void LocusRenderer::DrawGui()
{
	ImGui::ColorEdit4("rootColor", &_rootColor.x);
	ImGui::ColorEdit4("tipColor", &_tipColor.x);
}
// 描画処理
void LocusRenderer::Render(const RenderContext& rc)
{
	for (int i = _polygonStartIndex; i < _polygonEndIndex; ++i)
	{
		GetActor()->GetScene()->GetPrimitiveRenderer().AddVertex(
			_trailPositions[i].rootPosition, _rootColor);
		GetActor()->GetScene()->GetPrimitiveRenderer().AddVertex(
			_trailPositions[i].tipPosition, _tipColor);
	}
}
// 軌跡追加
void LocusRenderer::PushFrontVertex(const Vector3& rootWP, const Vector3& tipWP, const Vector3& velocity)
{
	_polygonStartIndex = 0;
	_trailPositions[0].rootPosition = rootWP;
	_trailPositions[0].tipPosition = tipWP;
	_trailPositions[0].velocity = velocity;
	_polygonEndIndex = std::clamp(_polygonEndIndex + 1, 0, MAX_POLYGON);
}
