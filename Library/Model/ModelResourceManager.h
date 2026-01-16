#pragma once

#include <memory>
#include <string>
#include <map>
#include <mutex>
#include "../Model/ModelResource.h"
#include "../../Library/Resource/ResourceManager.h"

// モデルのリソースマネージャー
class ModelResourceManager : public ResourceBase
{
public:
	struct ModelLoadInfo
	{
		// 複数スレッドでのアクセスを防ぐためのミューテックス
		std::mutex mutex;
		std::weak_ptr<ModelResource> resource;
	};

public:
	ModelResourceManager() = default;
	~ModelResourceManager() override {}

	// 名前取得
	std::string GetName() const override { return "ModelResourceManager"; }
	// ファイルパス取得
	std::string GetFilePath() const override { return ""; };
	// ファイル読み込み
	bool LoadFromFile() override { return true; }
	// ファイル保存
	bool SaveToFile() override { return true; }
	// モデルリソース読み込み
	std::shared_ptr<ModelResource> LoadModelResource(const char* filename);

	// GUiの表示
	void DrawGui() override;

private:
	using ModelMap = std::map<std::string, ModelLoadInfo>;
	ModelMap		_models;
};

// リソース設定
_REGISTER_RESOURCE(ModelResourceManager)
