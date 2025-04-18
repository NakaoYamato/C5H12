#pragma once

#include "Component.h"

class Rigidbody : public Component
{
public:
	Rigidbody() {}
	~Rigidbody() override {}
	// –¼‘OŽæ“¾
	const char* GetName() const override { return "Rigidbody"; }
	void Start() override;
	void Update(float elapsedTime) override;

};