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
	// 起動フラグが変化したときの処理
	// 子供にも伝播する
	virtual void OnChangedActive(bool isActive) override;

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

#pragma region アクセサ
	RectTransform& GetRectTransform() { return _rectTransform; }

	bool GetPropagateActiveChange() const { return _propagateActiveChange; }
	void SetPropagateActiveChange(bool propagate) { _propagateActiveChange = propagate; }
#pragma endregion

protected:
	/// <summary>
	/// トランスフォームGUI描画
	/// </summary>
	virtual void DrawTransformGui() override;

private:
	RectTransform	_rectTransform;
	RectTransform* _parentRectTransform = nullptr;

	// 起動フラグが変化したに子供にも伝播するためのフラグ
	bool			_propagateActiveChange = true;
};