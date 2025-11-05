#pragma once

#include "../../Library/Actor/ModelAttach/ModelAttachActor.h"

class ArmorActor : public Actor
{
public:
	~ArmorActor()override {}
	// 生成時処理
	void OnCreate() override;
	// Updateのあとによばれる更新時処理
	void OnLateUpdate(float elapsedTime)  override;
private:
	// モデルのトランスフォーム更新
	void UpdateModelTransform() override;

protected:
};