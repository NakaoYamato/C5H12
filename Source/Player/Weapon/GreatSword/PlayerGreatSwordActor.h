#pragma once

#include "../WeaponActor.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Collider/SphereCollider.h"

class PlayerGreatSwordActor : public WeaponActor
{
public:
	static const char* GetModelFilePath()
	{
		return "./Data/Model/Player/GreatSword/Weapon/Marcus' sword1.fbx";
	}
public:
	~PlayerGreatSwordActor()override {}

	// 生成時処理
	void OnCreate() override;
	// 開始時処理
	void OnStart() override;
	// 更新処理
	void OnUpdate(float elapsedTime) override;
private:
	std::weak_ptr<ModelRenderer> _modelRenderer;
	std::weak_ptr<ModelRenderer> _parentModelRenderer;
	std::weak_ptr<SphereCollider> _collider;

	float _rimLightFactor = 10.0f;
};