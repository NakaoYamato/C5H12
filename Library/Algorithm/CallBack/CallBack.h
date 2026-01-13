#pragma once

#include <stdexcept>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

// コールバック関数定義
template<class Return, typename... Args>
using CallBack = std::function<Return(Args...)>;

// コールバックハンドラクラス
template<class Return, typename... Args>
class CallBackHandler
{
public:
	CallBackHandler() = default;
	~CallBackHandler() = default;
	// コールバック関数登録
	void RegisterCallBack(const std::string& name, CallBack<Return, Args...> callback)
	{
		_callbacks[name] = callback;
	}
	// コールバック関数解除
	void UnregisterCallBack(const std::string& name)
	{
		_callbacks.erase(name);
	}
	// コールバック関数解除
	void UnregisterAllCallBacks()
	{
		_callbacks.clear();
	}

	// コールバック関数呼び出し
	Return Call(const std::string& name, Args... args)
	{
		auto it = _callbacks.find(name);
		if (it != _callbacks.end())
		{
			Return result = it->second(args...);
			return result;
		}
		throw std::runtime_error("Callback not found: " + name);
	}
	// コールバック関数呼び出し
	void CallVoid(const std::string& name, Args... args)
	{
		auto it = _callbacks.find(name);
		if (it != _callbacks.end())
		{
			it->second(args...);
		}
	}
	// コールバック関数呼び出し(全て)
	void Call(Args... args)
	{
		for (auto& [name, func] : _callbacks)
		{
			func(args...);
		}
	}

	// 名前一覧取得
	std::vector<std::string> GetCallBackNames() const
	{
		std::vector<std::string> names;
		for (const auto& [name, callback] : _callbacks)
		{
			names.push_back(name);
		}
		return names;
	}
private:
	std::unordered_map<std::string, std::function<Return(Args...)>> _callbacks;
};