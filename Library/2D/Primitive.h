#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "../../Library/Math/Vector.h"
#include "../../Library/Graphics/Shader.h"

class Primitive
{
public:
	// 定数定義
    static constexpr int VertexNum = 130;
    static constexpr int CircleMaxPolygonal = 64;

    // 構造体定義
    struct Vertex
    {
        Vector3 position;
        Vector4 color;
    };
public:
    Primitive(ID3D11Device* device);
    Primitive() = delete;
    ~Primitive() {}

    /// <summary>
    /// 矩形描画
    /// </summary>
    /// <param name="context"></param>
    /// <param name="pos">描画位置</param>
    /// <param name="size">幅高さ</param>
    /// <param name="center">基準点</param>
    /// <param name="angle">角度 radian</param>
    /// <param name="color">色</param>
    void Rect(ID3D11DeviceContext* context,
        const Vector2& pos, 
        const Vector2& size,
        const Vector2& center = Vector2::Zero, 
        float angle = (0.0f),
        const Vector4& color = Vector4::White) const;

    /// <summary>
    /// 線描画
    /// </summary>
    /// <param name="context"></param>
    /// <param name="from">始点</param>
    /// <param name="to">終点</param>
    /// <param name="color">色</param>
    /// <param name="width">幅</param>
    void Line(ID3D11DeviceContext* context,
        const Vector2& from, 
        const Vector2& to,
        const Vector4& color = Vector4::White, 
        float width = (1.0f)) const;

    /// <summary>
    /// 円描画
    /// </summary>
    /// <param name="context"></param>
    /// <param name="center">中心位置</param>
    /// <param name="radius">半径</param>
    /// <param name="scale">スケール</param>
    /// <param name="angle">角度</param>
    /// <param name="color">色</param>
    /// <param name="n">何角形か</param>
    void Circle(ID3D11DeviceContext* context,
        const Vector2& center, 
        float radius,
        const Vector2& scale = Vector2::One, 
        float angle = (0.0f),
        const Vector4& color = Vector4::White,
        int n = CircleMaxPolygonal) const;

    /// <summary>
    /// カプセル描画
    /// </summary>
    /// <param name="context"></param>
    /// <param name="from">始点</param>
    /// <param name="to">終点</param>
    /// <param name="scale">スケール</param>
    /// <param name="radius">半径</param>
    /// <param name="color">色</param>
    void Capsule(ID3D11DeviceContext* context,
        const Vector2& from, const Vector2& to,
        const Vector2& scale,
        const float& radius,
        const Vector4& color = Vector4::White) const;

private:
	VertexShader    _vertexShader;
	PixelShader     _pixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _vertexBuffer;
};