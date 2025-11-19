#include "ItemFunctions.h"

#include "../../Source/Common/Damageable.h"

void HealingPotionFunc::Execute(ItemData* item, Actor* user)
{
	if (!item || !user)
		return;
	auto damageable = user->GetComponent<Damageable>();
	if (!damageable)
		return;
	// ƒpƒ‰ƒ[ƒ^Žæ“¾
	float healAmount = 0.0f;
	float requiredTime = 0.0f;
	auto it = item->parameters.find("HealAmount");
	if (it != item->parameters.end())
	{
		healAmount = std::get<float>(it->second);
	}
	it = item->parameters.find("RequiredTime");
	if (it != item->parameters.end())
	{
		requiredTime = std::get<float>(it->second);
	}
	// ‰ñ•œˆ—
	damageable->Heal(healAmount);
}

ItemData::ParameterMap HealingPotionFunc::GetParameterKeys()
{
	ItemData::ParameterMap p;
	p["HealAmount"] = 0.0f;
	p["RequiredTime"] = 0.0f;
	return p;
}
