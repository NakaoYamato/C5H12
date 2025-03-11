#include "PlayerActor.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

// 開始処理
void PlayerActor::Start()
{
	GetTransform().SetLengthScale(0.01f);
	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>("./Data/Model/Player/2025_03_09_Warrior.fbx");
	auto animator = AddComponent<Animator>(modelRenderer->GetModel());

	// 基底クラスの初期化
	Actor::Start();
}
