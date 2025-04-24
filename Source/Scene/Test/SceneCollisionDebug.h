#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneCollisionDebug : public Scene
{
public:
	SceneCollisionDebug() {}
	~SceneCollisionDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"CollisionDebug"; }

	//初期化
	void Initialize()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneCollisionDebug>();
	}
};

// メニューバーに登録
_REGISTER_SCENE_MANAGER(SceneCollisionDebug, u8"CollisionDebug");