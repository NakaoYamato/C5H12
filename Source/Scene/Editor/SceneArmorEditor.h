#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneArmorEditor : public Scene
{
public:
	SceneArmorEditor() {}
	~SceneArmorEditor()override {}
	// 名前取得
	const char* GetName()const { return u8"ArmorEditor"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Editor; }
	//初期化
	void OnInitialize()override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneArmorEditor>();
	}
};

// メニューバーに登録
_REGISTER_SCENE(SceneArmorEditor)
