#include "PlayerGreatSwordActor.h"

#include "../../Library/Component/ModelRenderer.h"

void PlayerGreatSwordActor::OnCreate()
{
	// モデル読み込み
	LoadModel(GetModelFilePath());

	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>();
}
