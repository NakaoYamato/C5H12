#pragma once

#include "../../Source/InGame/InputManager.h"

class ChestInput : public InputControllerBase
{
public:
	ChestInput() {}
	~ChestInput() override {}
	// –¼‘Oæ“¾
	const char* GetName() const override { return "ChestInput"; }
	// GUI•`‰æ
	void DrawGui() override;
protected:
	// XVˆ—
	void OnUpdate(float elapsedTime)  override;
};
