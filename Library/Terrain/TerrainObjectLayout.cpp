#include "TerrainObjectLayout.h"

#include <imgui.h>

// GUI描画処理
void TerrainObjectLayout::DrawGui()
{
	if (ImGui::TreeNode(u8"登録しているモデル"))
	{
		for (auto& [path, model] : _models)
		{
			if (ImGui::TreeNode(path.c_str()))
			{
				if (model)
				{
					model->DrawGui();
				}
				ImGui::Separator();
				if (ImGui::Button(u8"モデルの削除"))
				{
					// モデルを削除
					RemoveModel(path);
					ImGui::TreePop();
					break; // 削除後はループを抜ける
				}
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"配置情報"))
	{
		for (auto& [index, layout] : _layouts)
		{
			if (ImGui::TreeNode(("Layout " + std::to_string(index)).c_str()))
			{
				ImGui::Text(u8"参照モデルパス: %s", layout.modelPath.c_str());
				ImGui::Combo("Collision Type", reinterpret_cast<int*>(&layout.collisionType), "None\0Box\0Sphere\0Capsule\0Mesh\0\0");
				ImGui::DragFloat3("Position", &layout.localPosition.x, 0.01f);
				ImGui::DragFloat3("Rotation", &layout.rotation.x, 0.01f);
				ImGui::DragFloat3("Size", &layout.size.x, 0.01f);
				if (ImGui::Button(u8"削除"))
				{
					// 配置情報を削除
					_layouts.erase(index);
					ImGui::TreePop();
					break; // 削除後はループを抜ける
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::Button(u8"リセット"))
	{
		// モデルと配置情報をリセット
		_models.clear();
		_layouts.clear();
		_currentObjectIndex = 0; // オブジェクトインデックスをリセット
	}
}
// モデルを追加
void TerrainObjectLayout::AddModel(ID3D11Device* device, const std::string& filepath)
{
	// 以前に同じモデルが登録されている場合は何もしない
	auto it = _models.find(filepath);
	if (it != _models.end())
	{
		return; // 既に登録されているモデル
	}

	// モデルを追加
	_models[filepath] = std::make_shared<Model>(device, filepath.c_str());
}
// 配置情報を追加
int TerrainObjectLayout::AddLayout(const std::string& modelPath,
	UpdateType updateType,
	CollisionType collisionType,
	const Vector3& position,
	const Vector3& rotation,
	const Vector3& size,
	const Vector3& collisionOffset,
	const Vector4& collisionParameter)
{
	// 配置情報を追加
	_layouts[_currentObjectIndex].modelPath = modelPath;
	_layouts[_currentObjectIndex].updateType = updateType;
	_layouts[_currentObjectIndex].collisionType = collisionType;
	_layouts[_currentObjectIndex].localPosition = position;
	_layouts[_currentObjectIndex].rotation = rotation;
	_layouts[_currentObjectIndex].size = size;
	_layouts[_currentObjectIndex].collisionOffset = collisionOffset;
	_layouts[_currentObjectIndex].collisionParameter = collisionParameter;
	_currentObjectIndex++;
	return _currentObjectIndex - 1; // 追加した配置情報のインデックスを返す
}
// 配置情報が登録されているか確認
const TerrainObjectLayout::LayoutData* TerrainObjectLayout::FindLayout(int index) const
{
	auto it = _layouts.find(index);
	if (it != _layouts.end())
	{
		return &_layouts.at(index); // 配置情報が存在する
	}
	return nullptr;
}
// モデルの削除
void TerrainObjectLayout::RemoveModel(const std::string& path)
{
	// モデルを削除
	_models.erase(path);

	// 削除後、配置情報からも参照を削除
	for (auto it = _layouts.begin(); it != _layouts.end();)
	{
		if (it->second.modelPath == path)
		{
			it = _layouts.erase(it); // 配置情報からも削除
		}
		else
		{
			++it; // 次の要素へ
		}
	}
}
// 書き出し
void TerrainObjectLayout::Export(nlohmann::json* jsonData)
{
	(*jsonData)["ModelsSize"] = _models.size();
	int modelIndex = 0;
	for (const auto& [path, model] : _models)
	{
		(*jsonData)["Model" + std::to_string(modelIndex)] = path;
		modelIndex++;
	}

	(*jsonData)["LayoutsSize"] = _layouts.size();
	int number = 0;
	for (auto& [index, layout] : _layouts)
	{
		// 書き出しの都合上、mapのキーとmapの順番の両方を保存する必要があるため
		// numberがmapを順に登録する数値、indexをmapのキーとして使用する
		(*jsonData)["Layout" + std::to_string(number)]["Key"] = index; // キーを保存
		layout.Export(("Layout" + std::to_string(number)).c_str(), jsonData);
		number++;
	}
}
// 読み込み
void TerrainObjectLayout::Import(ID3D11Device* device, const nlohmann::json& jsonData)
{
	if (jsonData.contains("ModelsSize"))
	{
		size_t modelSize = jsonData["ModelsSize"].get<size_t>();
		for (size_t modelIndex = 0; modelIndex < modelSize; ++modelIndex)
		{
			std::string modelPath = jsonData["Model" + std::to_string(modelIndex)].get<std::string>();
			_models[modelPath] = std::make_shared<Model>(device, modelPath.c_str());
		}
	}

	if (jsonData.contains("LayoutsSize"))
	{
		size_t layoutSize = jsonData["LayoutsSize"].get<size_t>();
		int maxKey = -1; // 最大キーを取得するための変数
		for (size_t number = 0; number < layoutSize; ++number)
		{
			int key = jsonData["Layout" + std::to_string(number)]["Key"].get<int>(); // キーを取得
			_layouts[key].Import(("Layout" + std::to_string(number)).c_str(), jsonData);
			if (key > maxKey)
				maxKey = key; // 最大キーを更新
		}
		_currentObjectIndex = maxKey + 1; // 現在のオブジェクトインデックスを更新
	}
}
// 書き出し
void TerrainObjectLayout::LayoutData::Export(const char* label, nlohmann::json* jsonData)
{
	(*jsonData)[label]["modelPath"] = modelPath;
	(*jsonData)[label]["updateType"] = updateType;
	(*jsonData)[label]["collisionType"] = collisionType;
	(*jsonData)[label]["localPosition.x"] = localPosition.x;
	(*jsonData)[label]["localPosition.y"] = localPosition.y;
	(*jsonData)[label]["localPosition.z"] = localPosition.z;
	(*jsonData)[label]["rotation.x"] = rotation.x;
	(*jsonData)[label]["rotation.y"] = rotation.y;
	(*jsonData)[label]["rotation.z"] = rotation.z;
	(*jsonData)[label]["size.x"] = size.x;
	(*jsonData)[label]["size.y"] = size.y;
	(*jsonData)[label]["size.z"] = size.z;
	(*jsonData)[label]["collisionOffset.x"] = collisionOffset.x;
	(*jsonData)[label]["collisionOffset.y"] = collisionOffset.y;
	(*jsonData)[label]["collisionOffset.z"] = collisionOffset.z;
	(*jsonData)[label]["collisionParameter.x"] = collisionParameter.x;
	(*jsonData)[label]["collisionParameter.y"] = collisionParameter.y;
	(*jsonData)[label]["collisionParameter.z"] = collisionParameter.z;
	(*jsonData)[label]["collisionParameter.w"] = collisionParameter.w;
}
// 読み込み
void TerrainObjectLayout::LayoutData::Import(const char* label, const nlohmann::json& jsonData)
{
	if (jsonData.contains(label))
	{
		modelPath = jsonData[label]["modelPath"].get<std::string>();
		//updateType = static_cast<UpdateType>(jsonData[label]["updateType"].get<int>());
		collisionType = static_cast<CollisionType>(jsonData[label]["collisionType"].get<int>());
		localPosition.x = jsonData[label]["localPosition.x"].get<float>();
		localPosition.y = jsonData[label]["localPosition.y"].get<float>();
		localPosition.z = jsonData[label]["localPosition.z"].get<float>();
		rotation.x = jsonData[label]["rotation.x"].get<float>();
		rotation.y = jsonData[label]["rotation.y"].get<float>();
		rotation.z = jsonData[label]["rotation.z"].get<float>();
		size.x = jsonData[label]["size.x"].get<float>();
		size.y = jsonData[label]["size.y"].get<float>();
		size.z = jsonData[label]["size.z"].get<float>();
		collisionOffset.x = jsonData[label]["collisionOffset.x"].get<float>();
		collisionOffset.y = jsonData[label]["collisionOffset.y"].get<float>();
		collisionOffset.z = jsonData[label]["collisionOffset.z"].get<float>();
		collisionParameter.x = jsonData[label]["collisionParameter.x"].get<float>();
		collisionParameter.y = jsonData[label]["collisionParameter.y"].get<float>();
		collisionParameter.z = jsonData[label]["collisionParameter.z"].get<float>();
		collisionParameter.w = jsonData[label]["collisionParameter.w"].get<float>();
	}
}
