#include "ArmorEditActor.h"

#include <Mygui.h>

void ArmorEditActor::OnCreate()
{
	// コンポーネント追加
	_modelRenderer = this->AddComponent<ModelRenderer>();
}

void ArmorEditActor::OnStart()
{
	_armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");

	auto parent = GetParent();
	if (parent == nullptr)
		return;
	_parentModelRenderer = parent->GetComponent<ModelRenderer>();

	// データ構築
	BuildData(_armorIndex);
}

void ArmorEditActor::OnLateUpdate(float elapsedTime)
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

void ArmorEditActor::OnDrawGui()
{
	auto armorManager = _armorManager.lock();
	if (!armorManager)
		return;
	auto parentModelRenderer = _parentModelRenderer.lock();
	if (parentModelRenderer == nullptr)
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
			ImGui::Separator();

			int index = 0;
			for (auto& data : armorManager->GetArmorDataList(_type))
			{
				// 選択中の防具
				bool active = index == _armorIndex;
				if (ImGui::RadioButton(data.name.c_str(), active))
				{
					// データ構築
					BuildData(index);
				}
				index++;
			}
			if (ImGui::RadioButton(u8"なし", _armorIndex == -1))
			{
				// データ構築
				BuildData(-1);
			}
			ImGui::Separator();

			if (_armorIndex >= 0 && _armorIndex < armorManager->GetArmorDataList(_type).size())
			{
				if (auto data = armorManager->GetArmorData(_type, static_cast<size_t>(_armorIndex)))
				{
					if (ImGui::TreeNode(u8"非表示メッシュ"))
					{
						for (auto& parentMaterial : parentModelRenderer->GetMaterials())
						{
							bool hidden = false;
							auto iter = std::find(
								data->hiddenMeshes.begin(),
								data->hiddenMeshes.end(),
								parentMaterial.GetName());
							// 非表示メッシュか確認
							if (iter != data->hiddenMeshes.end())
								hidden = true;

							if (ImGui::Checkbox(parentMaterial.GetName().c_str(), &hidden))
							{
								// 以前の非表示メッシュ解除
								auto oldData = armorManager->GetArmorData(_type, static_cast<size_t>(_armorIndex));
								if (oldData)
								{
									for (auto& name : oldData->hiddenMeshes)
									{
										parentModelRenderer->SetMeshHidden(name, false);
									}
								}

								if (hidden)
								{
									// 要素を追加
									data->hiddenMeshes.push_back(parentMaterial.GetName());
								}
								else
								{
									// 要素を削除
									data->hiddenMeshes.erase(iter);
								}

								// 再構築
								BuildData(_armorIndex);

								break;
							}
							//parentModelRenderer->SetMeshHidden(name, hidden);
						}
						ImGui::TreePop();
					}
				}
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void ArmorEditActor::BuildData(int index)
{
	auto parentModelRenderer = _parentModelRenderer.lock();
	if (parentModelRenderer == nullptr)
		return;
	auto armorManager = _armorManager.lock();
	if (!armorManager)
		return;

	// 以前の非表示メッシュ解除
	auto oldData = armorManager->GetArmorData(_type, index);
	if (oldData)
	{
		for (auto& name : oldData->hiddenMeshes)
		{
			parentModelRenderer->SetMeshHidden(name, false);
		}
	}

	_armorIndex = index;
	if (index < 0)
	{
		// モデル読み込み
		LoadModel("");
		return;
	}

	auto nextData = armorManager->GetArmorData(_type, static_cast<size_t>(index));
	if (nextData == nullptr || nextData->modelFilePath.empty())
	{
		// モデル読み込み
		LoadModel("");
		return;
	}
	// モデル読み込み
	LoadModel(nextData->modelFilePath.c_str());

	// 非表示メッシュ設定
	for (auto& name : nextData->hiddenMeshes)
	{
		parentModelRenderer->SetMeshHidden(name, true);
	}
}
