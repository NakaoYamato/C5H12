#pragma once

#include "../WyvernActor.h"
#include "WyvernBreathController.h"

class WyvernBreathActor : public Actor
{
public:
	WyvernBreathActor() {}
	~WyvernBreathActor() override {}
	// 生成時処理
	void OnCreate() override;

	// ブレスコントローラー取得
	std::shared_ptr<WyvernBreathController> GetBreathController() const {
		return _breathController.lock();
	}
private:
	// ブレスのコントローラー
	std::weak_ptr<WyvernBreathController> _breathController;
};