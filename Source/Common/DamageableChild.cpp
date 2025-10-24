#include "DamageableChild.h"

// ダメージを与える
bool DamageableChild::AddDamage(float damage, Vector3 hitPosition, bool networkData)
{
	// 親のDamageableにダメージを与える
	if (auto parent = _parent.lock())
	{
		bool res = parent->AddDamage(damage, hitPosition, networkData);
		if (res)
			this->_totalDamage += damage;
		return res;
	}
	return false;
}
