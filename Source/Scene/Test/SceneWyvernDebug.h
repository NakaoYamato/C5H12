#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneWyvernDebug : public Scene
{
public:
	SceneWyvernDebug() {}
	~SceneWyvernDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"WyvernDebug"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Debug; }
	//初期化
	void OnInitialize()override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneWyvernDebug>();
	}
};

// メニューバーに登録
_REGISTER_SCENE(SceneWyvernDebug)