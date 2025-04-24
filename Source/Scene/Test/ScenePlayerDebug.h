#pragma once

#include "../../Library/Scene/SceneManager.h"

class ScenePlayerDebug : public Scene
{
public:
	ScenePlayerDebug() {}
	~ScenePlayerDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"PlayerDebug"; }

	//初期化
	void Initialize()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<ScenePlayerDebug>();
	}
};

// メニューバーに登録
_REGISTER_SCENE_MANAGER(ScenePlayerDebug, u8"PlayerDebug");