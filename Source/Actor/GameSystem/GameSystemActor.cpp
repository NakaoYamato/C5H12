#include "GameSystemActor.h"

#include <imgui.h>

void GameSystemActor::OnCreate()
{
	Actor::OnCreate();

	_clientConnection = AddComponent<ClientConnection>();
}

void GameSystemActor::Start()
{
	Actor::Start();

    _networkMediator.SetClientCollback(_clientConnection);
}

void GameSystemActor::DrawGui()
{
    Actor::DrawGui();
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"ÉçÉO"))
		{
			auto logs = _networkMediator.GetLogs();
			ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 470), ImGuiWindowFlags_NoTitleBar);
			for (std::string message : logs) {
				ImGui::Text(u8"%s", message.c_str());
			}
			ImGui::EndChild();
			ImGui::Spacing();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}
