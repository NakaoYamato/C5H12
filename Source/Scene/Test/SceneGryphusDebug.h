#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneGryphusDebug : public Scene
{
public:
	SceneGryphusDebug() {}
	~SceneGryphusDebug() override {}
	// 名前取得
	const char* GetName() const override { return u8"GryphusDebug"; }
	// 階層取得
	SceneMenuLevel GetLevel() const override { return SceneMenuLevel::Debug; }
	// 初期化
	void OnInitialize() override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneGryphusDebug>();
	}
};
// メニューバーに登録
_REGISTER_SCENE(SceneGryphusDebug)