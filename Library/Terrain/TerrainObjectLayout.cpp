#include "TerrainObjectLayout.h"

#include <imgui.h>

// GUI描画処理
void TerrainObjectLayout::DrawGui()
{
	if (ImGui::TreeNode(u8"登録しているモデル"))
	{
		for (auto& [index, modelData] : _models)
		{
			if (ImGui::TreeNode(("Model " + std::to_string(index)).c_str()))
			{
				ImGui::Text("Filepath: %s", modelData->filepath.c_str());
				if (modelData->model)
				{
					modelData->model->DrawGui();
				}
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"配置情報"))
	{
		for (size_t i = 0; i < _layouts.size(); i++)
		{
			auto& layout = _layouts[i];
			if (ImGui::TreeNode(("Layout " + std::to_string(i)).c_str()))
			{
				ImGui::Text("Model Index: %d", layout.modelIndex);
				ImGui::DragFloat3("Position", &layout.position.x, 0.1f);
				ImGui::DragFloat3("Rotation", &layout.rotation.x, 0.1f);
				ImGui::DragFloat3("Size", &layout.size.x, 0.1f);
				if (ImGui::Button(u8"削除"))
				{
					// 配置情報を削除
					_layouts.erase(_layouts.begin() + i);
					ImGui::TreePop();
					break; // 削除後はループを抜ける
				}
				ImGui::TreePop();
			}
		}
		if (ImGui::Button(u8"配置情報の追加"))
		{
			// 新しい配置情報を追加
			AddLayout(0, Vector3::Zero, Vector3::Zero, Vector3::One);
		}
		ImGui::TreePop();
	}
}
// モデルを追加
void TerrainObjectLayout::AddModel(ID3D11Device* device, const std::string& filepath)
{
	// 以前に同じモデルが登録されている場合は何もしない
	for (auto& [index, modelData] : _models)
	{
		if (modelData->filepath == filepath)
		{
			return; // 既に登録されているモデル
		}
	}

	// モデルを追加
	_models[_currentModelIndex] = std::make_unique<ModelData>();
	_models[_currentModelIndex]->model = std::make_unique<Model>(device, filepath.c_str());
	_models[_currentModelIndex]->filepath = filepath;
	// 現在のモデルインデックスを更新
	_currentModelIndex++;
}
// 配置情報を追加
void TerrainObjectLayout::AddLayout(int modelIndex, const Vector3& position, const Vector3& rotation, const Vector3& size)
{
	// 配置情報を追加
	LayoutData layoutData;
	layoutData.modelIndex = modelIndex;
	layoutData.position = position;
	layoutData.rotation = rotation;
	layoutData.size = size;
	_layouts.push_back(std::move(layoutData));
}
