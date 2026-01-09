#include "MenuUIActor.h"

#include <imgui.h>

// 生成時処理
void MenuUIActor::OnCreate()
{
	UIActor::OnCreate();
	_spriteRenderer = GetComponent<SpriteRenderer>();
	// スプライトレンダラー側では描画を行わない

	// ルートウィジェット生成
	_rootWidget = std::make_shared<MenuWidget>();
	_rootWidget->SetOwner(this);
}

// 更新時処理
void MenuUIActor::OnUpdate(float elapsedTime)
{
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
		if (ImGui::BeginTabItem(u8"メニューUI"))
		{
			_rootWidget->DrawGui();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

// 起動フラグが変化したときの処理
void MenuUIActor::OnChangedActive(bool isActive)
{
	if (!isActive)
	{
		// 非アクティブになったらフォーカスを外す
		_focusedWidget = nullptr;
		_selectableWidgets.clear();
	}
	else
	{
		// アクティブになったらルートウィジェットにフォーカスを当てる
		_focusedWidget = _rootWidget.get();

		// 選択可能なウィジェットを列挙
		_selectableWidgets.clear();
		auto& childWidgets = _rootWidget->GetChildren();
		for (const auto& weakChild : childWidgets)
		{
			if (auto child = weakChild.lock())
			{
				if (child->CanSelect())
				{
					_selectableWidgets.push_back(child.get());
				}
			}
		}
	}
}

#pragma region MenuWidget
void MenuWidget::Setup()
{
	_onSetup.Call(this);
}

void MenuWidget::Update(float elapsedTime)
{
	_onUpdate.Call(this, elapsedTime);
}

void MenuWidget::Render(const RenderContext& rc)
{
	// 設定しているスプライトを描画
	if (!_spriteNames.empty())
	{
		auto spriteRenderer = _owner->GetSpriteRenderer();
		for (const auto& spriteName : _spriteNames)
		{
			spriteRenderer->SpriteRender(spriteName, rc);
		}
	}

	_onRender.Call(this, rc);
}

void MenuWidget::DrawGui()
{
}

bool MenuWidget::CanActive()
{
	bool result = true;
	auto names = _canActive.GetCallBackNames();
	for (const auto& name : names)
	{
		if (!_canActive.Call(name, this))
		{
			result = false;
		}
	}
	return result;
}

bool MenuWidget::CanSelect()
{
	bool result = true;
	auto names = _canSelect.GetCallBackNames();
	for (const auto& name : names)
	{
		if (!_canSelect.Call(name, this))
		{
			result = false;
		}
	}
	return result;
}

void MenuWidget::OnActive()
{
	_isActive = true;
	_onActive.Call(this);
}

void MenuWidget::OnSelect()
{
	_onSelect.Call(this);
}

void MenuWidget::OnDeactive()
{
	_isActive = false;
	_onDeactive.Call(this);
}

// ファイル読み込み
bool MenuWidget::LoadFromFile(nlohmann::json* json) const
{
	return false;
}

// ファイル保存
bool MenuWidget::SaveToFile(nlohmann::json* json)
{
	return false;
}

// 子供追加
std::shared_ptr<MenuWidget> MenuWidget::AddChild(
	const std::string& name,
	const std::vector<std::string>& spriteNames)
{
	auto child = std::make_shared<MenuWidget>();
	child->_parent = shared_from_this();
	child->_owner = _owner;
	child->_name = name;
	child->_spriteNames = spriteNames;
	_children.push_back(child);
	return child;
}
#pragma endregion
