#pragma once

#include "../../Library/Component/Component.h"

class EnvironmentDestroyer : public Component
{
public:
	EnvironmentDestroyer() {}
	~EnvironmentDestroyer() override {}
	// 名前取得
	const char* GetName() const override { return "EnvironmentDestroyer"; }

	// オブジェクトとの接触した瞬間時の処理
	void OnContactEnter(CollisionData& collisionData) override;
};