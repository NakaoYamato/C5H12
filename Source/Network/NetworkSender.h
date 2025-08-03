#pragma once

#include "../../Library/Component/Component.h"

#include <NetworkStructDefine.h>

class NetworkSender : public Component
{
public:
    NetworkSender() {}
    ~NetworkSender() {}
    const char* GetName() const override { return "NetworkSender"; }

protected:


private:
    float _sendInterval = 0.1f; // 送信間隔
    float _sendTimer = 0.0f; // 送信タイマー
};