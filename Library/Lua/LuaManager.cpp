#include "LuaManager.h"

#include "../../Library/Actor/Actor.h"
#include "../../Library/Scene/Scene.h"

LuaManager::LuaManager()
{
    // 標準ライブラリの展開
    _lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);

    // エンジン共通の型や関数を登録
    RegisterEngineAPI();
}

bool LuaManager::DoFile(const std::string& path)
{
    auto result = _lua.script_file(path);
    if (!result.valid()) {
        sol::error err = result;
        // ここでログ出力（ImGuiのコンソール等）
        return false;
    }
    return true;
}

void LuaManager::RegisterEngineAPI()
{
    // Vector3 などの数学クラスの登録
    _lua.new_usertype<Vector3>("Vector3",
        sol::constructors<Vector3(), Vector3(float, float, float)>(),
        "x", &Vector3::x, "y", &Vector3::y, "z", &Vector3::z
    );

    // Actorクラスの登録 (Actor.h を参考に)
    _lua.new_usertype<Actor>("Actor",
        "GetName", &Actor::GetName,
        "GetTransform", &Actor::GetTransform,
        "SetIsActive", &Actor::SetIsActive,
        "IsActive", &Actor::IsActive
    );
}
