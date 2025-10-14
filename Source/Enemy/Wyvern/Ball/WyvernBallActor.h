#pragma once

#include "../WyvernActor.h"
#include "WyvernBallController.h"

class WyvernBallActor : public Actor
{
public:
	WyvernBallActor() {}
	~WyvernBallActor() override {}
	// 生成時処理
	void OnCreate() override;
	// 火球コントローラー取得
	std::shared_ptr<WyvernBallController> GetBallController() const {
		return _ballController.lock();
	}
private:
	// 火球のコントローラー
	std::weak_ptr<WyvernBallController> _ballController;
};
