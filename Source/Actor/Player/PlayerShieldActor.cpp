#include "PlayerShieldActor.h"

#include "../../Library/Component/ModelRenderer.h"

void PlayerShieldActor::OnCreate()
{
	GetTransform().SetLengthScale(0.05f);
	GetTransform().SetPosition(Vector3(0.0f, 0.1f, -0.1f));
	GetTransform().SetRotation(Vec3ConvertToRadians(Vector3(0.0f, -90.0f, 80.0f)));
	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>("./Data/Model/Player/Shield/Shield.fbx");
}
