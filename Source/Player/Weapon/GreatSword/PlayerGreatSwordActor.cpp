#include "PlayerGreatSwordActor.h"

#include "../../Library/Component/ModelRenderer.h"

void PlayerGreatSwordActor::OnCreate()
{
	WeaponActor::OnCreate();
	// モデル読み込み
	LoadModel(GetModelFilePath());

	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>();

	_locusRootLocalPosition = Vector3(0.0f, -20.0f, 0.0f);
	_locusTipLocalPosition = Vector3(0.0f, -130.0f, 0.0f);
}
