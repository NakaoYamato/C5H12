#pragma once

#include <memory>
#include <string>
#include <map>
#include <mutex>
#include "../Resource/ModelResource.h"

// モデルのリソースマネージャー
class ModelResourceManager
{
private:
	ModelResourceManager() {}
	~ModelResourceManager() {}

public:
	// 唯一のインスタンス取得
	static ModelResourceManager& Instance()
	{
		static ModelResourceManager instance;
		return instance;
	}

	// モデルリソース読み込み
	std::shared_ptr<ModelResource> LoadModelResource(const char* filename);

	// GUiの表示
	void DrawGui();

private:
	using ModelMap = std::map<std::string, std::weak_ptr<ModelResource>>;
	ModelMap		_models;

	// 複数スレッドでのアクセスを防ぐためのミューテックス
	std::mutex _mutex;

	bool _showGui = false;
};