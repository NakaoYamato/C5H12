#pragma once

#include "../SceneManager.h"

class SceneInstancingModelDebug : public Scene
{
public:
	SceneInstancingModelDebug() {}
	~SceneInstancingModelDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"InstancingModelDebug"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Debug; }
	//初期化
	void OnInitialize()override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneInstancingModelDebug>();
	}
private:
	std::shared_ptr<Model> _model0;
	std::shared_ptr<Model> _model1;
	std::shared_ptr<Model> _model2;
};

// メニューバーに登録
_REGISTER_SCENE(SceneInstancingModelDebug)