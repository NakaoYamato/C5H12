#include "TitleWyvernActor.h"

#include "../../Library/Scene/Scene.h"

// 生成時処理
void TitleWyvernActor::OnCreate()
{
	// モデル読み込み
	auto model = LoadModel("./Data/Model/Enemy/Dragons/Wyvern.fbx");

	GetTransform().SetLengthScale(0.01f);
	GetTransform().SetScale(1.5f);

	// コンポーネント追加
	AddComponent<ModelRenderer>();
	_animator = AddComponent<Animator>();
}

// 開始時処理
void TitleWyvernActor::OnStart()
{
	if (auto animator = _animator.lock())
	{
		if (!animator->IsPlayAnimation())
		{
			animator->PlayAnimation("Idle03Shake", true);
        }
    }
}

