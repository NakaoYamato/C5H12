#pragma once

#include "../SceneManager.h"

class SceneModelEditor : public Scene
{
public:
	SceneModelEditor() {}
	~SceneModelEditor()override {}

	// 名前取得
	const char* GetName()const { return u8"ModelEditor"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Editor; }

	//初期化
	void Initialize()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneModelEditor>();
	}
};

// メニューバーに登録
_REGISTER_SCENE(SceneModelEditor)