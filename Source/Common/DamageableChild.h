#pragma once

#include "Damageable.h"

class DamageableChild : public Damageable
{
public:
	DamageableChild(std::weak_ptr<Damageable> parent) : _parent(parent)
	{
	}
	// 名前取得
	const char* GetName() const override { return "DamageableChild"; }
	virtual ~DamageableChild() = default;

	// ダメージを与える
	bool AddDamage(float damage, Vector3 hitPosition, bool networkData = false) override;
private:
	std::weak_ptr<Damageable> _parent;
};