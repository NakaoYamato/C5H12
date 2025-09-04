#pragma once

#include <NetworkStructDefine.h>

#include "../../Library/Component/Component.h"

class NetworkSender : public Component
{
public:
    NetworkSender() {}
    ~NetworkSender() {}
    const char* GetName() const override { return "NetworkSender"; }

	virtual Network::CharacterMove GetMoveData() = 0;
	virtual Network::CharacterApplyDamage GetApplyDamageData() = 0;
};