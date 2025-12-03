#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/Common/Targetable.h"

class SafetyZone : public Component
{
public:
    SafetyZone() {}
    ~SafetyZone() override {}
    // 名前取得
    const char* GetName() const override { return "SafetyZone"; }

    // GUI描画
    void DrawGui() override;

    float GetSafetyTimer() const { return _sefetyTimer; }
private:
    // セーフティタイマー
    float _sefetyTimer = 1.0f;
};