#include "ModelResourceManager.h"
#include <filesystem>
#include <imgui.h>
#include "../Graphics/Graphics.h"

std::shared_ptr<ModelResource> ModelResourceManager::LoadModelResource(const char* filename)
{
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);

	// すでに読み込まれていたらリソースを返す
	for (auto& model : _models)
	{
		if (model.first == filename)
		{
			if (!model.second.expired())
				return model.second.lock();
		}
	}

	// 新規モデルリソース読み込み
	std::shared_ptr<ModelResource> resource = std::make_shared<ModelResource>();
	resource->Load(filename);

	// 登録
	_models[filename] = resource;

	return resource;
}

// GUiの表示
void ModelResourceManager::DrawGui()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			ImGui::Checkbox(u8"モデルリソース", &_showGui);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (_showGui)
	{
		if (ImGui::Begin(u8"モデルリソースマネージャー"))
		{
			if (ImGui::TreeNode(u8"読み込んだモデル"))
			{
				for (const auto& [str, reource] : _models)
				{
					ImGui::Text(u8"%s", str.c_str());
				}

				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
}
