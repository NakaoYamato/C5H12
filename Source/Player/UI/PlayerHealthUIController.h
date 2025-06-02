#pragma once

#include "../../Source/Mediator/CanvasMediator.h"
#include "../../Source/Common/Damageable.h"

class PlayerHealthUIController : public UIController
{
public:
	PlayerHealthUIController(bool isUserControlled, std::shared_ptr<Damageable> damageable) :
		_isUserControlled(isUserControlled),
		_damageable(damageable) {}
	~PlayerHealthUIController() override {}
	// 名前取得
	const char* GetName() const override { return "PlayerHPUIController"; }
	// 開始処理
	void Start() override;
	// 削除処理
	void Deleted() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

private:
	// ユーザーが操作するプレイヤーか
	const bool _isUserControlled = true;
	std::weak_ptr<CanvasMediator> _canvasMediator;
	std::weak_ptr<Damageable> _damageable;
};