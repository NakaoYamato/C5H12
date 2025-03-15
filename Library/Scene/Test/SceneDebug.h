#pragma once

#include "../SceneManager.h"

class SceneDebug : public Scene
{
public:
	SceneDebug() {}
	~SceneDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"Debug"; }

	//初期化
	void Initialize()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneDebug>();
	}
};

// メニューバーに登録
REGISTER_SCENE_MANAGER(SceneDebug, u8"デバッグ");