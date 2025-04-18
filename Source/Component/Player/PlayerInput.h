#pragma once

#include "../../Library/Component/Component.h"

class PlayerInput : public Component
{
public:
	PlayerInput() {}
	~PlayerInput() override {}

	// –¼‘OŽæ“¾
	const char* GetName() const override { return "PlayerInput"; }

	void Start() override;

	void Update(float elapsedTime) override;

private:

};