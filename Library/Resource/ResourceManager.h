#pragma once

#include "ResourceBase.h"

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

	// リソース取得(型指定)
	template<class T>
	std::shared_ptr<T> GetResourceAs(const std::string& name)
	{
		auto resource = GetResource(name);
		if (resource)
		{
			return std::dynamic_pointer_cast<T>(resource);
		}
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

private:
	// リソースマップ
	std::unordered_map<std::string, std::shared_ptr<ResourceBase>> _resourceMap;
};

template<class T>
inline ResourceRegister<T>::ResourceRegister()
{
	ResourceManager::Instance().RegisterResource<T>();
}
