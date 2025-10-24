#pragma once

#include "Damageable.h"

class DamageableChild : public Damageable
{
public:
	DamageableChild(std::weak_ptr<Damageable> parent) : _parent(parent)
	{
	}
	virtual ~DamageableChild() = default;

	// ƒ_ƒ[ƒW‚ğ—^‚¦‚é
	bool AddDamage(float damage, Vector3 hitPosition, bool networkData = false) override;
private:
	std::weak_ptr<Damageable> _parent;
};