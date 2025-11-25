#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}
	// 名前取得
	const char* GetName() const override { return u8"Title"; }
	// 階層取得
	SceneMenuLevel GetLevel() const override { return SceneMenuLevel::Game; }
	// 初期化
	void OnInitialize() override;
	// 更新処理
	void OnUpdate(float elapsedTime) override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneTitle>();
	}
};

// メニューバーに登録
_REGISTER_SCENE(SceneTitle)