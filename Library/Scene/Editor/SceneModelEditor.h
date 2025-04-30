#pragma once

#include "../SceneManager.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

#include "../../Library/Physics/Collision/AnimationCollisionData.h"

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
	// メニューバーのGUI描画
	void DrawMenuBarGui();
	// 編集GUI描画
	void DrawEditGui();
	// アニメーション編集GUI描画
	void DrawEditAnimationGui();
private:
	std::string _filepath;
	std::string _currentDirectory;
	std::string _relativePath;

	std::weak_ptr<Actor> _modelActor;
	std::weak_ptr<ModelRenderer> _modelRenderer;
	std::weak_ptr<Animator> _animator;

	AnimationCollisionData _animCollisionData;

	// デバッグ用
	Vector4 _nodeColor = _VECTOR4_RED;
	std::vector<const char*> _nodeNames;
};

// メニューバーに登録
_REGISTER_SCENE(SceneModelEditor)