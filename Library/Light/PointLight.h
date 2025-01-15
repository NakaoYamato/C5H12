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
    const Data& GetData()const { return data_; }
    Data* GetDataPointer() { return &data_; }
    const Vector4& GetPosition()const { return data_.position; }
    const Vector4& GetColor()const { return data_.color; }
    float GetRange()const { return data_.range; }

    void SetPosition(const Vector4& v) { data_.position = v; }
    void SetColor(const Vector4& v) { data_.color = v; }
    void SetRange(const float r) { data_.range = r; }
    void SetActiveFlag(bool b) { data_.isAlive = b == true ? 1 : 0; }
private:
    Data data_;
};