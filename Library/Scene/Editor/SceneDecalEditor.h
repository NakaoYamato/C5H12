#pragma once

#include "../SceneManager.h"

class SceneDecalEditor : public Scene
{
public:
	SceneDecalEditor() {}
	~SceneDecalEditor() override {}
	// 名前取得
	const char* GetName() const override { return u8"DecalEditor"; }
	// 階層取得
	SceneMenuLevel GetLevel() const override { return SceneMenuLevel::Editor; }
	// 初期化
	void OnInitialize() override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneDecalEditor>();
	}
};

// メニューバーに登録
_REGISTER_SCENE(SceneDecalEditor)