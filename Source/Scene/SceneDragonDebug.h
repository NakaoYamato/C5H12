#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneDragonDebug : public Scene
{
public:
	SceneDragonDebug() {}
	~SceneDragonDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"DragonDebug"; }

	//初期化
	void Initialize()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneDragonDebug>();
	}
};

// メニューバーに登録
_REGISTER_SCENE_MANAGER(SceneDragonDebug, u8"DragonDebug");