#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <string>
#include <memory>
#include <vector>

#include "../../Library/DebugSupporter/DebugSupporter.h"

// 前方宣言
class Actor;
class Scene;

class LuaManager
{
public:
    LuaManager();

    // スクリプトファイルの実行（ホットリロード対応）
    bool DoFile(const std::string& path);

    // 特定の関数を呼び出す（例：OnUpdate）
    template<typename... Args>
    void CallLuaFunction(const std::string& funcName, Args&&... args) 
    {
        sol::protected_function func = _lua[funcName];
        if (func.valid()) {
            auto result = func(std::forward<Args>(args)...);
            if (!result.valid()) {
                sol::error err = result;
                Debug::Output::String(err.what());
            }
        }
    }

    sol::state& GetState() { return _lua; }

private:
    sol::state _lua;

    /// <summary>
    /// エンジンの基本クラスをLuaに登録する
    /// </summary>
    void RegisterEngineAPI();
};