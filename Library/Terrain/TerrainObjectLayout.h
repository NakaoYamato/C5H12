#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "../../Library/Math/Vector.h"
#include "../../Library/Model/Model.h"

class TerrainObjectLayout
{
public:
	// 登録しているモデル情報
	struct ModelData
	{
		// モデル
		std::unique_ptr<Model> model;
		// モデルのパス
		std::string filepath = "";
	};
	// 配置情報
	struct LayoutData
	{
		// モデルの参照番号
		int modelIndex = -1;
		// 配置位置
		Vector3 position = Vector3::Zero;
		// 配置回転
		Vector3 rotation = Vector3::Zero;
		// 配置サイズ
		Vector3 size = Vector3::One;
	};
public:
	TerrainObjectLayout() = default;
	~TerrainObjectLayout() = default;

	// GUI描画処理
	void DrawGui();
	// モデルを追加
	void AddModel(ID3D11Device* device, const std::string& filepath);
	// 配置情報を追加
	void AddLayout(int modelIndex, const Vector3& position, const Vector3& rotation, const Vector3& size);
	// モデルデータを取得
	const std::unordered_map<int, std::unique_ptr<ModelData>>& GetModels() const { return _models; }
	// モデルデータを取得
	const ModelData* GetModels(int index) { return _models[index].get(); }
	// 配置データを取得
	const std::vector<LayoutData>& GetLayouts() const { return _layouts; }
private:
	// 登録しているモデルデータ
	std::unordered_map<int, std::unique_ptr<ModelData>> _models;
	// モデルの配置情報
	std::vector<LayoutData> _layouts;
	// 現在のモデルのインデックス
	int _currentModelIndex = 0;
};