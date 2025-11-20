#pragma once

#include "../Actor.h"

#include "../../Library/Model/ModelResource.h"

class ModelAttachActor : public Actor
{
public:
	~ModelAttachActor()override {}

	// 初期設定
	void Initialize(Actor* owner, const ModelResource::Node* parentNode)
	{
		// 親情報を保存
		SetParent(owner);
		_parentNode = parentNode;
	}
	// 遅延更新時処理
	void OnLateUpdate(float elapsedTime) override;

	std::string GetParentName() override
	{
		return _parentNode ? _parentNode->name : "";
	}
protected:
	// トランスフォーム更新
	void UpdateTransform() override
	{
		_transform.UpdateTransform(&_parentNode->worldTransform);
	}

	// ギズモ描画
	void DrawGuizmo() override;

protected:
	const ModelResource::Node* _parentNode = nullptr;
};