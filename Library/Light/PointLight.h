#pragma once

#include "../Math/Vector.h"

// ポイントライト
class PointLight
{
public:
    static constexpr int POINT_LIGHT_MAX = 8;

    struct Data
    {
        // 位置
        Vector4 position{};
        // 色
        Vector4 color{};
        // 範囲
        float range{};
        // 使用フラグ 1 使用
        int isAlive = 0;
        Vector2 dummy{};
    };
public:
    // デバッグGui表示
    void DrawGui();

    // デバッグ表示
    void DebugRender();

    // アクセサ
    const Data& GetData()const { return _data; }
    Data* GetDataPointer() { return &_data; }
    const Vector4& GetPosition()const { return _data.position; }
    const Vector4& GetColor()const { return _data.color; }
    float GetRange()const { return _data.range; }

    void SetPosition(const Vector4& v) { _data.position = v; }
    void SetColor(const Vector4& v) { _data.color = v; }
    void SetRange(const float r) { _data.range = r; }
    void SetActiveFlag(bool b) { _data.isAlive = b == true ? 1 : 0; }
private:
    Data _data;
};