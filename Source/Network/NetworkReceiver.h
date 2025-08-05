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
	bool IsActive() const { return _active; }
	void SetActive(bool active) { _active = active; }
    int GetManagerId() const { return _managerId; }
    void SetManagerId(int managerId) { _managerId = managerId; }
    EventBus& GetEventBus() { return _eventBus; }
#pragma endregion

private:
	bool _active = true; // アクティブ状態
    // 管理者のID
    int _managerId = -1;
    EventBus _eventBus;
};