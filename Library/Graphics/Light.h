#pragma once

#include "../Math/Vector.h"

// ポイントライト
class PointLight
{
public:
    static constexpr int POINT_LIGHT_MAX = 8;

    // 位置
    Vector4 position{};
    // 色
    Vector4 color = _VECTOR4_WHITE;
    // 範囲
    float range{};
    // 使用フラグ 1 使用
    int isAlive = 0;
    Vector2 dummy{};
};