#pragma once

#include "../UIActor.h"
#include "../../Library/Algorithm/UIWidget/UIWidget.h"

class MenuWidget;

class MenuUIActor : public UIActor
{
public:
	MenuUIActor() = default;
	~MenuUIActor() override {}
	// 生成時処理
	void OnCreate() override;
	// 更新時処理
	void OnUpdate(float elapsedTime) override;
	// 3D描画後の描画時処理
	void OnDelayedRender(const RenderContext& rc) override;
	// トランスフォーム更新
	void UpdateTransform() override;

	// GUI描画時処理
	void OnDrawGui() override;
private:
	UIWidget _rootWidget;

	// GUIで選択中のウィジェット
	std::shared_ptr<UIWidget> _selectedWidget = nullptr;
};
