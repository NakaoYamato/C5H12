#pragma once

#include "../../Library/Scene/SceneManager.h"

#include "../../Library/Component/ModelRenderer.h"

class SceneLookAtDebug : public Scene
{
public:
	SceneLookAtDebug() {}
	~SceneLookAtDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"LookAtDebug"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Debug; }

	//初期化
	void Initialize()override;
	// 更新処理
	void Update(float elapsedTime)override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneLookAtDebug>();
	}
private:
	std::weak_ptr<Actor>		 _targetActor;
	std::weak_ptr<Model>		 _model;
	std::weak_ptr<ModelRenderer> _modelRenderer;
	// 初期姿勢時の頭ノードのローカル空間前方向
	Vector3		_headLocalForward = Vector3::Zero;
};

// メニューバーに登録
_REGISTER_SCENE(SceneLookAtDebug)