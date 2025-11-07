#pragma once

#include "../../Library/Actor/ModelAttach/ModelAttachActor.h"
#include "ArmorManager.h"

class ArmorActor : public Actor
{
public:
	ArmorActor(ArmorType type, int armorIndex = 0) :
		_type(type),
		_armorIndex(armorIndex)
	{}
	~ArmorActor()override {}
	// 生成時処理
	void OnCreate() override;
	// Updateのあとによばれる更新時処理
	void OnLateUpdate(float elapsedTime)  override;
	// GUI描画処理
	void OnDrawGui() override;
private:
	// モデルのトランスフォーム更新
	void UpdateModelTransform() override;

	// データ構築
	void BuildData(ArmorManager* manager, int index);

protected:
	ArmorType _type;
	int _armorIndex = 0;
	ArmorData* _armorData = nullptr;
};