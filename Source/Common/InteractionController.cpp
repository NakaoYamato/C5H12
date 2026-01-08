#include "InteractionController.h"

#include <Mygui.h>

// GUI描画処理
void InteractionController::DrawGui()
{
	auto CallBackDrawGui = [&](const char* label, CallBackHandler<bool, Actor*>& handler)
		{
			if (ImGui::TreeNode(u8"選択可能かどうかのコールバック"))
			{
				auto names = handler.GetCallBackNames();
				for (auto& name : names)
				{
					ImGui::Text(name.c_str());
				}
				ImGui::TreePop();
			}
		};

	CallBackDrawGui(u8"選択可能かどうかのコールバック", _isSelectableCallback);
	ImGui::Separator();
	CallBackDrawGui(u8"使用可能かどうかのコールバック", _isUsableCallback);
	ImGui::Separator();

	auto CallBackVoidDrawGui = [&](const char* label, CallBackHandler<void, Actor*>& handler)
		{
			if (ImGui::TreeNode(u8"選択時のコールバック"))
			{
				auto names = handler.GetCallBackNames();
				for (auto& name : names)
				{
					ImGui::Text(name.c_str());
				}
				ImGui::TreePop();
			}
		};
	CallBackVoidDrawGui(u8"選択時のコールバック", _onSelectCallback);
	ImGui::Separator();
	CallBackVoidDrawGui(u8"使用時のコールバック", _onUseCallback);
}

#pragma region コールバック呼び出し
// 選択可能か
bool InteractionController::IsSelectable(Actor* target)
{
	bool result = true;
	auto names = _isSelectableCallback.GetCallBackNames();
	for (auto& name : names)
	{
		if (!_isSelectableCallback.Call(name, target))
		{
			result = false;
		}
	}

	return result;
}

// 選択時
void InteractionController::OnSelect(Actor* target)
{
	_onSelectCallback.Call(target);
}

// 使用可能か
bool InteractionController::IsUsable(Actor* target)
{
	bool result = true;
	auto names = _isUsableCallback.GetCallBackNames();
	for (auto& name : names)
	{
		if (!_isUsableCallback.Call(name, target))
		{
			result = false;
		}
	}

	return result;
}

// 使用時
void InteractionController::OnUse(Actor* target)
{
	_onUseCallback.Call(target);
}
#pragma endregion
