#include "ArmorActor.h"

#include "../../Library/Component/ModelRenderer.h"

#include <Mygui.h>

void ArmorActor::OnCreate()
{
	// コンポーネント追加
	_modelRenderer = this->AddComponent<ModelRenderer>();
}

void ArmorActor::OnStart()
{
	auto armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");
	if (armorManager == nullptr)
		return;

	auto parent = GetParent();
	if (parent == nullptr)
		return;
	_parentModelRenderer = parent->GetComponent<ModelRenderer>();

	// データ構築
	BuildData(armorManager.get(), _armorIndex);
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

	// プレイヤーのマテリアル情報を反映
	Vector4 color = *_parentModelRenderer.lock()->GetMaterials().at(0).GetParameterF4("bodyColor");
	for (auto& material : _modelRenderer.lock()->GetMaterials())
	{
		// マテリアルをプレイヤーと同じものにする
		if (material.GetShaderName() != "Player")
		{
			auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
			material.SetShaderName("Player");
			// シェーダー変更時はパラメータも初期化
			material.SetParameterMap(modelShaderResource->GetShaderParameterKey(
				_modelRenderer.lock()->GetRenderType(),
				"Player",
				true));
		}

		material.SetParameter("bodyColor", color);
	}
}

// GUI描画処理
void ArmorActor::OnDrawGui()
{
	auto armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");
	if (armorManager == nullptr)
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
			if (_armorData != nullptr)
			{
				if (ImGui::TreeNode(u8"非表示メッシュ"))
				{
					for (auto& [name, flag] : parentModelRenderer->GetHiddenMeshMap())
					{
						bool hidden = false;

						// 現在の非表示メッシュリストに存在するか
						auto str = 
							std::find(_armorData->hiddenMeshes.begin(), _armorData->hiddenMeshes.end(), name);
						if (str != _armorData->hiddenMeshes.end())
						{
							hidden = true;
						}

						if (ImGui::Checkbox(name.c_str(), &hidden))
						{
							// 非表示メッシュリストに存在するなら削除
							if (str != _armorData->hiddenMeshes.end())
							{
								_armorData->hiddenMeshes.erase(str);
								parentModelRenderer->SetMeshHidden(name, false);
							}
							// 存在しないなら追加
							else
							{
								_armorData->hiddenMeshes.push_back(name);
								parentModelRenderer->SetMeshHidden(name, true);
							}
						}
					}

					ImGui::TreePop();
				}
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
	auto parentModelRenderer = _parentModelRenderer.lock();
	if (parentModelRenderer == nullptr)
		return;

	// 以前の非表示メッシュ解除
	if (_armorData != nullptr)
	{
		for (auto& name : _armorData->hiddenMeshes)
		{
			parentModelRenderer->SetMeshHidden(name, false);
		}

	}

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

	// 非表示メッシュ設定
	for (auto& name : _armorData->hiddenMeshes)
	{
		parentModelRenderer->SetMeshHidden(name, true);
	}
}
