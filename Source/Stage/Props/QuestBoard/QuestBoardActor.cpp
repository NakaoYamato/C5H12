#include "QuestBoardActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Collider/SphereCollider.h"
#include "../../Library/Component/Collider/BoxCollider.h"
#include "../../Source/Common/InteractionController.h"

#include "QuestBoardController.h"

// 生成時処理
void QuestBoardActor::OnCreate()
{
	// モデルロード
	//LoadModel("./Data/Model/Stage/Props/Chest/ChestBottom.fbx");

	// コンポーネント追加
	auto modelRenderer = this->AddComponent<ModelRenderer>();
	auto controller = this->AddComponent<QuestBoardController>();
	auto interactionController = this->AddComponent<InteractionController>();

	// コライダー追加
	auto bodyCollider = this->AddCollider<BoxCollider>();
	auto judgeCollider = this->AddCollider<SphereCollider>();

	// パラメータ設定
	modelRenderer->SetRenderType(ModelRenderType::Static);
	bodyCollider->SetLayer(CollisionLayer::Stage);
	bodyCollider->SetLayerMask(GetCollisionLayerMaskExcept(CollisionLayer::Stage));
	bodyCollider->SetTrigger(false);
	bodyCollider->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
	bodyCollider->SetRadius(Vector3(1.0f, 1.0f, 0.3f));
	judgeCollider->SetLayer(CollisionLayer::Stage);
	judgeCollider->SetLayerMask(GetCollisionLayerMaskExcept(CollisionLayer::Stage));
	judgeCollider->SetTrigger(true);
	judgeCollider->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
}
