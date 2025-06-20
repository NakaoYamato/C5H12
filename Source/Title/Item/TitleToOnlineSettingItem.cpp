#include "TitleToOnlineSettingItem.h"

#include "../../Library/Input/Input.h"
#include "../../Source/Scene/Game/SceneGame.h"

#include <imgui.h>

std::string TitleToOnlineSettingItem::NetworkAddress = "127.0.0.1";

TitleToOnlineSettingItem::TitleToOnlineSettingItem(TitleMediator* titleMediator, const std::string& itemName)
	: TitleItemBase(titleMediator, itemName)
{
}
// 更新処理
void TitleToOnlineSettingItem::Update(float elapsedTime)
{
}
// 描画
void TitleToOnlineSettingItem::Render(Scene* scene, const RenderContext& rc)
{
	// ImGuiで処理
	if (ImGui::Begin(u8"設定"))
	{
		ImGui::InputText(u8"ネットワークアドレス", &NetworkAddress);
		if (ImGui::Button(u8"決定"))
		{
			// ゲームシーンに移行
			SceneGame::NetworkEnabled = true;
			SceneGame::NetworkAddress = NetworkAddress;
			SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Game");
		}
	}
	ImGui::End();
}