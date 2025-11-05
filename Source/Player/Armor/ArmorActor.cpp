#include "ArmorActor.h"

#include "../../Library/Component/ModelRenderer.h"

void ArmorActor::OnCreate()
{
}

// Updateのあとによばれる更新時処理
void ArmorActor::OnLateUpdate(float elapsedTime)
{
	if (GetParent() == nullptr)
	{
		return;
	}

	auto model = GetModel().lock();
	auto parentModel = GetParent()->GetModel().lock();

	if (model == nullptr || parentModel == nullptr)
	{
		return;
	}

	// 親のモデルのボーン行列をこのモデルに適応
	for (auto& node : model->GetPoseNodes())
	{
		int index = parentModel->GetNodeIndex(node.name);
		if (index < 0)
			continue;

		auto& parentNode = parentModel->GetPoseNodes()[index];
		node.position = parentNode.position;
		node.rotation = parentNode.rotation;
		node.scale = parentNode.scale;
		node.localTransform = parentNode.localTransform;
		node.worldTransform = parentNode.worldTransform;
	}
}

// モデルのトランスフォーム更新
void ArmorActor::UpdateModelTransform()
{
	// ここでは処理しない
}
