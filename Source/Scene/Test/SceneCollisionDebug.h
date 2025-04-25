#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneCollisionDebug : public Scene
{
public:
	SceneCollisionDebug() {}
	~SceneCollisionDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"CollisionDebug"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Debug; }

	//初期化
	void Initialize()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneCollisionDebug>();
	}
};

// メニューバーに登録
_REGISTER_SCENE(SceneCollisionDebug)