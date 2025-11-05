#include "ResourceManager.h"

#include <imgui.h>

// Gui描画
void ResourceManager::DrawGui()
{
#if USE_IMGUI
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			ImGui::Checkbox(u8"リソースマネージャー", &_drawGUI);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (_drawGUI)
	{
		if (ImGui::Begin(u8"リソースマネージャー", &_drawGUI))
		{
			for (auto& [name, resource] : _resourceMap)
			{
				if (ImGui::TreeNode(name.c_str()))
				{
					ImGui::Text("FilePath: %s", resource->GetFilePath().c_str());
					ImGui::Separator();

					resource->DrawGui();

					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
	}
#endif
}
