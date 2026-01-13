#include "MenuUIActor.h"

#include <imgui.h>

// 生成時処理
void MenuUIActor::OnCreate()
{
	UIActor::OnCreate();

	// ルートウィジェット設定
	_rootWidget.SetName("Root");

	// ウィジェット追加例
	auto listView = std::make_shared<UIListView<int>>();
	listView->SetName("ListView");
	_rootWidget.AddChild(listView);

	// データを用意
	std::vector<int> list = {
		1, 2, 3, 4, 5
	};
	// リストビューにモデルを設定
	listView->SetModel(list, [](const int& data) -> std::shared_ptr<UIWidget>
		{
			auto widget = std::make_shared<UIWidget>();

			auto text = std::make_shared<UIText>();
			text->SetName(std::to_string(data));
			text->SetText(L"アイテム " + std::to_wstring(data));

			widget->AddChild(text);

			return widget;
		});

	_rootWidget.SetActive(true);
	listView->SetActive(true);
}

// 更新時処理
void MenuUIActor::OnUpdate(float elapsedTime)
{
	UIActor::OnUpdate(elapsedTime);
	_rootWidget.Update(elapsedTime);
}

// 3D描画後の描画時処理
void MenuUIActor::OnDelayedRender(const RenderContext& rc)
{
	UIActor::OnDelayedRender(rc);
	_rootWidget.Render(GetScene());
}

// トランスフォーム更新
void MenuUIActor::UpdateTransform()
{
	UIActor::UpdateTransform();
	_rootWidget.UpdateTransform(&GetRectTransform());
}

// GUI描画時処理
void MenuUIActor::OnDrawGui()
{
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"ウィジェット"))
		{
			if (_selectedWidget)
			{
				auto next = _selectedWidget->DrawGui();
				if (next)
					_selectedWidget = next;
			}
			else
				_selectedWidget = _rootWidget.DrawGui();
		
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
