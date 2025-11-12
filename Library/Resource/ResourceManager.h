#pragma once

#include "ResourceBase.h"

#include <cassert>
#include <unordered_map>
#include <memory>

// リソースマネージャーに登録するための構造体
template<class T>
struct ResourceRegister
{
	ResourceRegister();
};

// リソースマネージャーに登録するためのマクロ
// type			: ResourceBase継承クラス
#define _REGISTER_RESOURCE(type) \
static ResourceRegister<type> type##register;

//---------------------------------------------------
//  リソース管理クラス
//---------------------------------------------------
class ResourceManager
{
private:
	ResourceManager() {}
	~ResourceManager() {}
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;

public:
	// シングルトンインスタンス取得
	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// リソース取得(型指定)
	template<class T>
	std::shared_ptr<T> GetResourceAs(const std::string& name)
	{
		auto resource = std::dynamic_pointer_cast<T>(_resourceMap[name]);
		if (resource)
		{
			return resource;
		}
		// エラー通知
		assert(!"ResourceManager::GetResourceAs(): 指定の型のリソースが見つかりませんでした");
		return nullptr;
	}
	
	// リソース取得(型指定)
	template<class T>
	std::shared_ptr<T> GetResourceAs()
	{
		for (auto& pair : _resourceMap)
		{
			auto resource = std::dynamic_pointer_cast<T>(pair.second);
			if (resource)
			{
				return resource;
			}
		}
		// エラー通知
		assert(!"ResourceManager::GetResourceAs(): 指定の型のリソースが見つかりませんでした");
		return nullptr;
	}

	// 登録
	template<class T>
	void RegisterResource()
	{
		std::shared_ptr<ResourceBase> resource = std::make_shared<T>();
		// ファイル読み込み
		resource->LoadFromFile();
		_resourceMap[resource->GetName()] = resource;
	}

	// Gui描画
	void DrawGui();

private:
	// リソースマップ
	std::unordered_map<std::string, std::shared_ptr<ResourceBase>> _resourceMap;

	// デバッグGUI表示フラグ
	bool _drawGUI = false;
};

template<class T>
inline ResourceRegister<T>::ResourceRegister()
{
	ResourceManager::Instance().RegisterResource<T>();
}
