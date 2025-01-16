#pragma once

#include "../SceneManager.h"

#include "../../3D/Model.h"
class SceneDebug : public Scene
{
public:
	SceneDebug() {}
	~SceneDebug()override {}

	//初期化
	void Initialize()override;

	//終了化 
	void Finalize()override;

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render()override;

	// デバッグ用Gui描画
	void DrawGui()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneDebug>();
	}

	std::unique_ptr<Model> testModel;
};

// メニューバーに登録
REGISTER_SCENE_MANAGER(SceneDebug, u8"デバッグ");