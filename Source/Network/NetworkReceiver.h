#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Algorithm/EventBus/EventBus.h"

#include <NetworkStructDefine.h>

class NetworkReceiver : public Component
{
public:
    NetworkReceiver() {}
    ~NetworkReceiver() {}
    const char* GetName() const override { return "NetworkReceiver"; }

#pragma region アクセサ
    int GetManagerId() const { return _managerId; }
    void SetManagerId(int managerId) { _managerId = managerId; }
    EventBus& GetEventBus() { return _eventBus; }
#pragma endregion

private:
    // 管理者のID
    int _managerId = -1;
    EventBus _eventBus;
};