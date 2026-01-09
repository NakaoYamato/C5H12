#include "QuestBoardActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Collider/BoxCollider.h"
#include "../../Source/Common/InteractionController.h"

#include "QuestBoardController.h"

// 生成時処理
void QuestBoardActor::OnCreate()
{
	// モデルロード
	LoadModel("./Data/Model/Stage/Props/QuestBoard/QuestBoard.fbx");

	// コンポーネント追加
	auto modelRenderer = this->AddComponent<ModelRenderer>();
	auto controller = this->AddComponent<QuestBoardController>();
	auto interactionController = this->AddComponent<InteractionController>();

	// コライダー追加
	auto bodyCollider = this->AddCollider<BoxCollider>();
	auto judgeCollider = this->AddCollider<BoxCollider>();

	// パラメータ設定
	GetTransform().SetLengthScale(0.01f);
	modelRenderer->SetRenderType(ModelRenderType::Static);
	bodyCollider->SetLayer(CollisionLayer::Stage);
	bodyCollider->SetLayerMask(GetCollisionLayerMaskExcept(CollisionLayer::Stage));
	bodyCollider->SetTrigger(false);
	bodyCollider->SetPosition(Vector3(0.0f, 170.0f, 0.0f));
	bodyCollider->SetRadius(Vector3(1.8f, 1.5f, 0.2f));
	judgeCollider->SetLayer(CollisionLayer::Stage);
	judgeCollider->SetLayerMask(GetCollisionLayerMaskExcept(CollisionLayer::Stage));
	judgeCollider->SetTrigger(true);
	judgeCollider->SetPosition(Vector3(0.0f, 170.0f, -100.0f));
	judgeCollider->SetRadius(Vector3(1.8f, 1.5f, 0.5f));
}
