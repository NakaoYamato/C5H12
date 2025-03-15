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
    const Vector4& GetDirection()const { return _direction; }
    const Vector4& GetColor()const { return _color; }
    const Vector4& GetAmbientColor()const { return _ambientColor; }

    void SetDirection(const Vector4& v) { this->_direction = v; }
    void SetColor(const Vector4& v) { this->_color = v; }
    void SetAmbientColor(const Vector4& v) { this->_ambientColor = v; }
private:
    // 光源からの光の向き
    Vector4 _direction = { -0.2f,-0.45f,0.85f,0.0f };
    Vector4 _color = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 _ambientColor = { 1.0f,1.0f,1.0f,1.0f };

    // デバッグ用
    int _currentIndex = 0;
    // 光の始点
    Vector3 _lightStart = {};
    // 光の終点
    Vector3 _lightEnd = {};
};