#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneGame : public Scene
{
public:
    // ネットワーク有効フラグ
	static bool NetworkEnabled;
    // ネットワークアドレス
	static std::string NetworkAddress;

public:
    SceneGame() {}
    ~SceneGame() override {}
    // 名前取得
    const char* GetName() const override { return u8"Game"; }
    // 階層取得
    SceneMenuLevel GetLevel() const override { return SceneMenuLevel::Game; }
    // 初期化
    void OnInitialize() override;
    // 自身を新規のポインタで渡す
    std::shared_ptr<Scene> GetNewShared() override
    {
        return std::make_shared<SceneGame>();
    }

private:
    // 予め読み込むモデルのリスト
    std::vector<std::shared_ptr<Model>> _preloadModels;
};

// メニューバーに登録
_REGISTER_SCENE(SceneGame)