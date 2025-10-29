#pragma once

#include "../Actor.h"
#include "../../Library/Math/RectTransform.h"

class UIActor : public Actor
{
public:
	UIActor() {}
	virtual ~UIActor() override {}

	/// <summary>
	/// 生成時処理
	/// </summary>
	virtual void OnCreate() override;
	/// <summary>
	/// トランスフォーム更新
	/// </summary>
	virtual void UpdateTransform() override;

	/// <summary>
	/// 3D描画後の描画時処理
	/// </summary>
	/// <param name="rc"></param>
	virtual void OnDelayedRender(const RenderContext& rc) override;
#pragma region 親子関係
	/// <summary>
	/// 親設定
	/// </summary>
	/// <param name="parent"></param>
	virtual void SetParent(Actor* parent) override;
#pragma endregion

	RectTransform& GetRectTransform() { return _rectTransform; }
protected:
	/// <summary>
	/// トランスフォームGUI描画
	/// </summary>
	virtual void DrawTransformGui() override;

private:
	RectTransform	_rectTransform;
	RectTransform* _parentRectTransform = nullptr;
};