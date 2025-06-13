#pragma once

#include "../../Library/Scene/SceneManager.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

class SceneWyvernIKDebug : public Scene
{
public:
	SceneWyvernIKDebug() {}
	~SceneWyvernIKDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"WyvernIKDebug"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Debug; }

	//初期化
	void Initialize()override;
	// 更新処理
	void Update(float elapsedTime)override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneWyvernIKDebug>();
	}
private:
	void UpdateTwoBoneIK(float elapsedTime);
	void UpdateAnimationIK(float elapsedTime);

private:
	std::weak_ptr<Actor>		 _targetActor;
	std::weak_ptr<Model>		 _model;
	std::weak_ptr<ModelRenderer> _modelRenderer;
	std::weak_ptr<Animator>		 _animator;
	// ポールターゲット
	Vector3 _poleLocalPosition = Vector3::Up;

	float _lerpTimer = 0.0f;
	float _lerpSpeed = 10.0f;

	bool _calculateIK = false;
	bool _playerIK = false;
	bool _twoBoneIK = false;
};

// メニューバーに登録
_REGISTER_SCENE(SceneWyvernIKDebug)