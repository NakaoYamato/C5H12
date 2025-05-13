#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Component/ModelRenderer.h"

class DragonActor : public Actor
{
public:
	enum class ModelType
	{
		Brown,
		Green,
		Lava,
		Red,
		White,
	};

	enum class DrawHornType
	{
		Horn01,
		Horn02,
		Horn03,
		Horn04,
		Horn05,
		Horn06,
	};

public:
	~DragonActor()override {}

	// 生成時処理
	void OnCreate() override;
	// 開始関数
	void OnStart() override;
	// 更新処理
	void OnUpdate(float elapsedTime) override;
	// GUI描画
	void OnDrawGui() override;

	void SetModelType(ModelType type) { this->_modelType = type; }
private:
	// モデルのテクスチャを設定
	void SetModelTexture();

	// モデルのアニメーションを設定
	void SetModelAnimation();

	// 使用する角を設定
	void SetUseHorn();

private:
	std::weak_ptr<ModelRenderer> _modelRenderer;

	ModelType _modelType = ModelType::Lava;
	DrawHornType _drawHornType = DrawHornType::Horn01;
};