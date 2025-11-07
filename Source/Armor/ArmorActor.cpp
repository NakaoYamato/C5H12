#include "ArmorActor.h"

#include "../../Library/Component/ModelRenderer.h"

#include <Mygui.h>

void ArmorActor::OnCreate()
{
	auto armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");
	if (armorManager == nullptr)
		return;

	// データ構築
	BuildData(armorManager.get(), _armorIndex);

	// コンポーネント追加
	auto modelRenderer = this->AddComponent<ModelRenderer>();
}

// Updateのあとによばれる更新時処理
void ArmorActor::OnLateUpdate(float elapsedTime)
{
	if (GetParent() == nullptr)
	{
		return;
	}

	auto model = GetModel().lock();
	auto parentModel = GetParent()->GetModel().lock();

	if (model == nullptr || parentModel == nullptr)
	{
		return;
	}

	// 親のモデルのボーン行列をこのモデルに適応
	for (auto& node : model->GetPoseNodes())
	{
		int index = parentModel->GetNodeIndex(node.name);
		if (index < 0)
			continue;

		auto& parentNode = parentModel->GetPoseNodes()[index];
		node.position = parentNode.position;
		node.rotation = parentNode.rotation;
		node.scale = parentNode.scale;
		node.localTransform = parentNode.localTransform;
		node.worldTransform = parentNode.worldTransform;
	}
}

// GUI描画処理
void ArmorActor::OnDrawGui()
{
	auto armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");
	if (armorManager == nullptr)
		return;

	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"防具"))
		{
			ImGui::Text(u8"選択中の防具Index: %d", _armorIndex);
			if (_armorData != nullptr)
			{

			}

			ImGui::Separator();

			int index = 0;
			for (auto& data : armorManager->GetArmorDataList(_type))
			{
				bool active = index == _armorIndex;
				if (ImGui::RadioButton(data.name.c_str(), active))
				{
					// データ構築
					BuildData(armorManager.get(), index);
				}
				index++;
			}
			if (ImGui::RadioButton(u8"なし", _armorIndex == -1))
			{
				// データ構築
				BuildData(armorManager.get(), -1);
			}
			ImGui::Separator();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

// モデルのトランスフォーム更新
void ArmorActor::UpdateModelTransform()
{
	// ここでは処理しない
}

// データ構築
void ArmorActor::BuildData(ArmorManager* manager, int index)
{
	_armorIndex = index;
	_armorData = manager->GetArmorData(_type, _armorIndex);
	if (_armorData == nullptr)
	{
		// モデル読み込み
		LoadModel("");
		return;
	}
	// モデル読み込み
	LoadModel(_armorData->modelFilePath.c_str());
}
