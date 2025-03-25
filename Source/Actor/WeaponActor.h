#pragma once

#include "../../Library/Actor/Actor.h"

#include "../../Library/Resource/ModelResource.h"

class WeaponActor : public Actor
{
public:
	~WeaponActor()override {}

	// 初期設定
	void Initialize(Actor* owner, const ModelResource::Node* parentNode)
	{
		_owner = owner;
		_parentNode = parentNode;
	}
protected:
	// トランスフォーム更新
	void UpdateTransform() override
	{
		_transform.UpdateTransform(&_parentNode->worldTransform);
	}

protected:
	Actor* _owner = nullptr;
	const ModelResource::Node* _parentNode = nullptr;
};