#include "QuestBoardActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Collider/SphereCollider.h"
#include "../../Library/Component/Collider/BoxCollider.h"

#include "QuestBoardController.h"

// 生成時処理
void QuestBoardActor::OnCreate()
{
	// モデルロード
	//LoadModel("./Data/Model/Stage/Props/Chest/ChestBottom.fbx");

	// コンポーネント追加
	auto modelRenderer = this->AddComponent<ModelRenderer>();
	auto controller = this->AddComponent<QuestBoardController>();

	// コライダー追加
	auto bodyCollider = this->AddCollider<BoxCollider>();
	auto judgeCollider = this->AddCollider<SphereCollider>();

	// パラメータ設定
	modelRenderer->SetRenderType(ModelRenderType::Static);
	bodyCollider->SetLayer(CollisionLayer::Stage);
	bodyCollider->SetLayerMask(GetCollisionLayerMaskExcept(CollisionLayer::Stage));
	bodyCollider->SetTrigger(false);
}
