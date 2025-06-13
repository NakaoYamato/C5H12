#pragma once

#include "../../Library/Scene/SceneManager.h"

class ScenePlayerDebug : public Scene
{
public:
	ScenePlayerDebug() {}
	~ScenePlayerDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"PlayerDebug"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Debug; }
	//初期化
	void OnInitialize()override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<ScenePlayerDebug>();
	}
};

// メニューバーに登録
_REGISTER_SCENE(ScenePlayerDebug)