#pragma once

#include "../SceneManager.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

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

	// 更新処理
	void Update(float elapsedTime)override;

	// Gui描画処理
	void DrawGui()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneModelEditor>();
	}

private:
	std::string _filepath;
	std::string _currentDirectory;
	std::string _relativePath;

	std::weak_ptr<Actor> _modelActor;
	std::weak_ptr<ModelRenderer> _modelRenderer;
	std::weak_ptr<Animator> _animator;
};

// メニューバーに登録
_REGISTER_SCENE(SceneModelEditor)