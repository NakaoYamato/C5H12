#include "ModelResourceManager.h"
#include <filesystem>
#include <imgui.h>
#include "../Graphics/Graphics.h"

std::shared_ptr<ModelResource> ModelResourceManager::LoadModelResource(const char* filename)
{
	// すでに読み込まれていたらリソースを返す
	for (auto& model : _models)
	{
		if (model.first == filename)
		{
			auto& info = model.second;
			// スレッドセーフ
			std::lock_guard<std::mutex> modelLock(info.mutex);
			if (!info.resource.expired())
				return info.resource.lock();
		}
	}

	auto& info = _models[filename];
	// スレッドセーフ
	std::lock_guard<std::mutex> modelLock(info.mutex);
	// 新規モデルリソース読み込み
	std::shared_ptr<ModelResource> resource = std::make_shared<ModelResource>();
	resource->Load(filename);
	info.resource = resource;

	return resource;
}

// GUiの表示
void ModelResourceManager::DrawGui()
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
