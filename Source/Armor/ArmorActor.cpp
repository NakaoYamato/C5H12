#include "ArmorActor.h"

#include <Mygui.h>

void ArmorActor::OnCreate()
{
	// コンポーネント追加
	_modelRenderer = this->AddComponent<ModelRenderer>();
}

void ArmorActor::OnStart()
{
	_userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");

	auto parent = GetParent();
	if (parent == nullptr)
		return;
	_parentModelRenderer = parent->GetComponent<ModelRenderer>();

	// データがあるか確認
	UserDataManager::ArmorUserData* userData = _userDataManager.lock()->GetEquippedArmorData(_type);

	// データ構築
	BuildData(userData->GetBaseData(), _armorIndex);
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
	if (const Vector4* color = _parentModelRenderer.lock()->GetMaterials().at(0).GetParameterF4("bodyColor"))
	{
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

			material.SetParameter("bodyColor", *color);
		}
	}
}

// GUI描画処理
void ArmorActor::OnDrawGui()
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
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
			for (auto& data : userDataManager->GetAcquiredArmorDataList(_type))
			{
				if (data.GetBaseData() == nullptr)
					continue;

				// 選択中の防具
				bool active = index == _armorIndex;
				if (ImGui::RadioButton(data.GetBaseData()->name.c_str(), active))
				{
					// データ構築
					BuildData(data.GetBaseData(), index);
				}
				index++;
			}
			if (ImGui::RadioButton(u8"なし", _armorIndex == -1))
			{
				// データ構築
				BuildData(nullptr, -1);
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
void ArmorActor::BuildData(ArmorData* data, int index)
{
	auto parentModelRenderer = _parentModelRenderer.lock();
	if (parentModelRenderer == nullptr)
		return;
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// 以前の非表示メッシュ解除
	UserDataManager::ArmorUserData* userData = _userDataManager.lock()->GetEquippedArmorData(_type);
	if (userData && userData->GetBaseData() != nullptr)
	{
		for (auto& name : userData->GetBaseData()->hiddenMeshes)
		{
			parentModelRenderer->SetMeshHidden(name, false);
		}

	}

	userDataManager->SetEquippedArmorIndex(_type, index);
	userData = _userDataManager.lock()->GetEquippedArmorData(_type);

	_armorIndex = index;
	if (data == nullptr || data->modelFilePath.empty())
	{
		// モデル読み込み
		LoadModel("");
		return;
	}
	// モデル読み込み
	LoadModel(data->modelFilePath.c_str());

	// 非表示メッシュ設定
	for (auto& name : data->hiddenMeshes)
	{
		parentModelRenderer->SetMeshHidden(name, true);
	}
}
