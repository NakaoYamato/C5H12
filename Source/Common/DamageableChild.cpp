#include "DamageableChild.h"

// ダメージを与える
bool DamageableChild::AddDamage(float damage, Vector3 hitPosition, bool networkData)
{
	// ダメージを受ける判定のコールバック関数がfalseを返した場合はダメージを受けない
	if (_takeableDamageCallback)
		if (_takeableDamageCallback(damage, hitPosition) == false) return false;
	// 親のDamageableにダメージを与える
	if (auto parent = _parent.lock())
	{
		bool res = parent->AddDamage(damage, hitPosition, networkData);
		if (res)
		{
			this->_totalDamage += damage;
			// ダメージを受けたときのコールバック関数呼び出し
			if (_onDamageCallback)
				_onDamageCallback(damage, hitPosition);
		}
		return res;
	}
	return false;
}
