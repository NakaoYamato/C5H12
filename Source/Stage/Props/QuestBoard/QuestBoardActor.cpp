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
}
