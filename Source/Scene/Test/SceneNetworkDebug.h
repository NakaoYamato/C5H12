#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneNetworkDebug : public Scene
{
public:
	SceneNetworkDebug() {}
	~SceneNetworkDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"NetworkDebug"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Debug; }

	//初期化
	void Initialize()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneNetworkDebug>();
	}

private:
};

// メニューバーに登録
_REGISTER_SCENE(SceneNetworkDebug)