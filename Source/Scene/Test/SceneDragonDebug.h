#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneDragonDebug : public Scene
{
public:
	SceneDragonDebug() {}
	~SceneDragonDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"DragonDebug"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Debug; }
	//初期化
	void OnInitialize()override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneDragonDebug>();
	}
};

// メニューバーに登録
_REGISTER_SCENE(SceneDragonDebug)