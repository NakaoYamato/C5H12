#include "WeakActor.h"

#include "../../Library/Component/Collider/ModelCollider.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"
#include "../EnemyController.h"

void WeakActor::OnCreate()
{
	EnemyActor::OnCreate();

	// モデル読み込み
	auto model = LoadModel(GetModelFilePath());
	// モデルがシリアライズされていなければエラー
	assert(model.lock()->GetResource()->IsSerialized());

	GetTransform().SetLengthScale(0.01f);

	// コンポーネント追加
	auto _modelRenderer = AddComponent<ModelRenderer>();
	auto _animator = AddComponent<Animator>();
	auto enemyController = AddComponent<EnemyController>();

	// コライダー追加
	auto modelCollider = AddCollider<ModelCollider>();
	modelCollider->SetLayer(CollisionLayer::Hit);
}
