#pragma once

#include "../../Library/Component/Component.h"

#include "../../Source/Item/ItemManager.h"

class PlayerItemController : public Component
{
public:
	PlayerItemController() {}
	~PlayerItemController() override {}
	// –¼‘Oæ“¾
	const char* GetName() const override { return "PlayerItemController"; }
	// ¶¬ˆ—
	void OnCreate() override;
	// XVˆ—
	void Update(float elapsedTime) override;
	// GUI•`‰æ
	void DrawGui() override;

private:

};