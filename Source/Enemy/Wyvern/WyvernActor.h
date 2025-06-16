#pragma once

#include "../EnemyActor.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

#include "WyvernEnemyController.h"
#include "WyvernBehaviorController.h"

class WyvernActor : public EnemyActor
{
public:
	enum class TextureType
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
	WyvernActor() {}
	~WyvernActor()override {}
	// 生成時処理
	void OnCreate() override;
	// 更新処理
	void OnUpdate(float elapsedTime) override;
	// GUI描画
	void OnDrawGui() override;

#pragma region アクセサ
	// テクスチャを設定
	void SetModelType(TextureType type) { this->_textureType = type; }
	// 角を設定
	void SetDrawHornType(DrawHornType type) { this->_drawHornType = type; }
	// ビヘイビアツリーを実行するかどうか
	void SetIsExecuteBehaviorTree(bool execute) override;
#pragma endregion

private:
	// モデルのテクスチャを設定
	void SetModelTexture();
	// 使用する角を設定
	void SetUseHorn();

private:
	std::weak_ptr<ModelRenderer> _modelRenderer;
	std::weak_ptr<Animator> _animator;
	std::weak_ptr<WyvernEnemyController> _wyvernEnemyController;
	std::weak_ptr<WyvernBehaviorController> _wyvernBehaviorController;

	TextureType _textureType = TextureType::Lava;
	DrawHornType _drawHornType = DrawHornType::Horn01;
};