#pragma once

#include "../SceneManager.h"

class SceneDebug : public Scene
{
public:
	SceneDebug() {}
	~SceneDebug()override {}

	//初期化
	void Initialize()override;

	//終了化 
	void Finalize()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneDebug>();
	}
};

// メニューバーに登録
REGISTER_SCENE_MANAGER(SceneDebug, u8"デバッグ");