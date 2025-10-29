#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "../Math/Vector.h"

class Primitive
{
public:
    static constexpr int VertexNum = 130;
    static constexpr int CircleMaxPolygonal = 64;

public:
    Primitive(ID3D11Device* device);
    Primitive() = delete;
    ~Primitive() {}

    //--------------------------------------------------------------
    //  矩形描画
    //--------------------------------------------------------------
    //  const Vector2& pos      描画位置 (x, y)
    //  const Vector2& size     幅高さ   (w, h)
    //  const Vector2& center   基準点   (x, y)
    //  float angle             角度     (radian)
    //  const Vector4& color    色       (r, g, b, a) (0.0f ~ 1.0f)
    //  bool  world        true:ワールド座標に描画 false : スクリーン座標に描画
    //--------------------------------------------------------------
    void Rect(ID3D11DeviceContext* context,
        const Vector2& pos, 
        const Vector2& size,
        const Vector2& center = Vector2::Zero, 
        float angle = (0.0f),
        const Vector4& color = Vector4::White) const;

    //--------------------------------------------------------------
    //  線描画
    //--------------------------------------------------------------
    //  const Vector2& from     始点 (x, y)
    //  const Vector2& to       終点 (x, y)
    //  const Vector4& color    色   (r, g, b, a)
    //  float width             幅
    //  bool world         true:ワールド座標に描画 false : スクリーン座標に描画
    //--------------------------------------------------------------
    void Line(ID3D11DeviceContext* context,
        const Vector2& from, 
        const Vector2& to,
        const Vector4& color = Vector4::White, 
        float width = (1.0f)) const;

    //--------------------------------------------------------------
    //  円描画
    //--------------------------------------------------------------
    //  const Vector2& pos      中心位置 (x, y)
    //  float radius            半径
    //  const Vector2& scale    スケール
    //  float angle             角度
    //  const Vector4& color    色   (r, g, b, a)
    //  int n                   何角形か
    //  bool world              true:ワールド座標に描画 false : スクリーン座標に描画
    //--------------------------------------------------------------
    void Circle(ID3D11DeviceContext* context,
        const Vector2& center, 
        float radius,
        const Vector2& scale = Vector2::One, 
        float angle = (0.0f),
        const Vector4& color = Vector4::White,
        int n = CircleMaxPolygonal) const;

    //--------------------------------------------------------------
    //  カプセル描画
    //--------------------------------------------------------------
    //  const Vector2& from     始点 (x, y)
    //  const Vector2& to       終点 (x, y)
    //  const Vector2& scale    スケール
    //  float radius            半径
    //  const Vector4& color    色   (r, g, b, a)
    //--------------------------------------------------------------
    void Capsule(ID3D11DeviceContext* context,
        const Vector2& from, const Vector2& to,
        const Vector2& scale,
        const float& radius,
        const Vector4& color = Vector4::White);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _vertexBuffer;

    //--------------------------------------------------------------
    //  構造体定義
    //--------------------------------------------------------------
    struct Vertex { Vector3 position; Vector4 color; };
};