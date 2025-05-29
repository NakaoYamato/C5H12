#pragma once

#include "../SceneManager.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

#include "../../Library/Model/AnimationEvent.h"
#include "../../Library/Model/ModelCollision.h"

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
	// モデルGUI描画
	void DrawModelGui();
	// アニメーションイベントGUI描画
	void DrawAnimationEventGui();
	// モデル当たり判定GUI描画
	void DrawModelColliderGui();
	// アニメーション編集GUI描画
	void DrawEditAnimationGui();
	// アニメーション追加GUI描画
	void DrawAddAnimationGui();
	// テクスチャのGUI描画
    void DrawTextureGui();
private:
	std::string _filepath;
	std::string _currentDirectory;
	std::string _relativePath;

	std::weak_ptr<Actor> _modelActor;
	std::weak_ptr<ModelRenderer> _modelRenderer;
	std::weak_ptr<Animator> _animator;

	// アニメーションデータ
	AnimationEvent _animationEvent;
	// モデルの衝突判定
	ModelCollision _modelCollision;

	// 追加アニメーションモデル
	std::shared_ptr<Model> _animationModel;
	std::vector<bool> _addAnimationMap;

	// デバッグ用
	bool _showNode = true;
	Vector4 _nodeColor = Vector4::Red;
	bool _showCollision = true;

	int _selectingMeshIndex = -1;
	float _vertexPointRadius = 0.05f;
	Vector4 _vertexPointColor = Vector4::Gray;

	// 取り除くアニメーション名
	std::string _filterAnimationName;
};

// メニューバーに登録
_REGISTER_SCENE(SceneModelEditor)