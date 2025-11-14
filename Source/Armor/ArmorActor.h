#pragma once

#include "../../Library/Actor/ModelAttach/ModelAttachActor.h"
#include "../../Library/Component/ModelRenderer.h"

#include "../../Source/User/UserDataManager.h"

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
	// 開始時処理
	void OnStart() override;
	// Updateのあとによばれる更新時処理
	void OnLateUpdate(float elapsedTime)  override;
	// GUI描画処理
	void OnDrawGui() override;

	// データ構築
	void BuildData(ArmorData* data, int index);
private:
	// モデルのトランスフォーム更新
	void UpdateModelTransform() override;

protected:
	ArmorType _type;
	int _armorIndex = 0;

	std::weak_ptr<ModelRenderer> _modelRenderer;
	// 親のモデルレンダラー
	std::weak_ptr<ModelRenderer> _parentModelRenderer;
	// ユーザーデータマネージャー
	std::weak_ptr<UserDataManager> _userDataManager;
};