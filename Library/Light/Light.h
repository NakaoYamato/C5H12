#pragma once

#include "../Math/Vector.h"

// 定数バッファで使う光源
class Light
{
public:
    // デバッグGui表示
    void DrawGui();

    // デバッグ表示
    void DebugRender();

    // アクセサ
    const Vector4& GetDirection()const { return direction_; }
    const Vector4& GetColor()const { return color_; }
    const Vector4& GetAmbientColor()const { return ambientColor_; }

    void SetDirection(const Vector4& v) { this->direction_ = v; }
    void SetColor(const Vector4& v) { this->color_ = v; }
    void SetAmbientColor(const Vector4& v) { this->ambientColor_ = v; }
private:
    // 光源からの光の向き
    Vector4 direction_ = { -0.2f,-0.45f,0.85f,0.0f };
    Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 ambientColor_ = { 0.0f,0.0f,0.0f,1.0f };

    // デバッグ用
    int currentIndex_ = 0;
    // 光の始点
    Vector3 lightStart_ = {};
    // 光の終点
    Vector3 lightEnd_ = {};
};