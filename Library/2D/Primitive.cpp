#include "Primitive.h"
#include "../Graphics/GpuResourceManager.h"

Primitive::Primitive(ID3D11Device* device)
{
    //VertexBufferの作成
    Vertex vertices[VertexNum] = { Vector3::Zero, Vector4::Zero };
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(vertices);			// 頂点バッファのサイズ
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファ
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// GPU→読み取りのみ　CPU→書き込みのみ
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    if (FAILED(device->CreateBuffer(&bd, nullptr, &_vertexBuffer)))
    {
        assert(!"頂点バッファの作成に失敗(Primitive)");
        return;
    }

    // 頂点宣言
    // 入力レイアウトの定義
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,     D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    //	頂点シェーダーの読み込み
    GpuResourceManager::CreateVsFromCso(device, "./Data/Shader/HLSL/Primitive/PrimitiveVS.cso", _vertexShader.GetAddressOf(),
        _inputLayout.GetAddressOf(), layout, numElements);

    //	ピクセルシェーダーの作成
    GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/HLSL/Primitive/PrimitivePS.cso", _pixelShader.GetAddressOf());
}

void Primitive::Rect(ID3D11DeviceContext* context,
    const Vector2& pos, const Vector2& size,
    const Vector2& center, float angle,
    const Vector4& color) const
{
    if (size.x == 0.0f || size.y == 0.0f) return;

    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    Vertex vertices[4]{};
    vertices[0] = { Vector3(-0.0f, -0.0f, 0), color };
    vertices[1] = { Vector3(+1.0f, -0.0f, 0), color };
    vertices[2] = { Vector3(-0.0f, +1.0f, 0), color };
    vertices[3] = { Vector3(+1.0f, +1.0f, 0), color };

    float sinValue = sinf(angle);
    float cosValue = cosf(angle);
    for (int i = 0; i < 4; i++) {
        vertices[i].position.x *= size.x;
        vertices[i].position.y *= size.y;
        vertices[i].position.x -= center.x;
        vertices[i].position.y -= center.y;

        float rx = vertices[i].position.x;
        float ry = vertices[i].position.y;
        vertices[i].position.x = rx * cosValue - ry * sinValue;
        vertices[i].position.y = rx * sinValue + ry * cosValue;
        vertices[i].position.x += pos.x;
        vertices[i].position.y += pos.y;

        vertices[i].position.x = -1.0f + vertices[i].position.x * 2 / viewport.Width;
        vertices[i].position.y = 1.0f - vertices[i].position.y * 2 / viewport.Height;
    }

    D3D11_MAPPED_SUBRESOURCE msr;
    context->Map(_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, vertices, sizeof(vertices));
    context->Unmap(_vertexBuffer.Get(), 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->IASetInputLayout(_inputLayout.Get());
    context->VSSetShader(_vertexShader.Get(), NULL, 0);
    context->PSSetShader(_pixelShader.Get(), NULL, 0);

    context->Draw(4, 0);
}

void Primitive::Line(ID3D11DeviceContext* context,
    const Vector2& from, const Vector2& to,
    const Vector4& color, float width) const
{
    if (width <= 0.0f) return;

    Vector2 v1(from.x, from.y);
    Vector2 v2(to.x, to.y);
    float w = Vector2::Length(v2 - v1);
    float h = width;
    float cx = w * 0.5f;
    float cy = h * 0.5f;
    float x = (from.x + to.x) * 0.5f;
    float y = (from.y + to.y) * 0.5f;
    float angle = atan2f(to.y - from.y, to.x - from.x);

    Rect(context, Vector2(x, y), Vector2(w, h), Vector2(cx, cy), angle, color);
}

void Primitive::Circle(ID3D11DeviceContext* context,
    const Vector2& center, float radius,
    const Vector2& scale, float angle,
    const Vector4& color, int n) const
{
    if (n < 3 || radius <= 0.0f) return;
    if (n > CircleMaxPolygonal) n = CircleMaxPolygonal;//最大チェック

    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    Vertex vertices[130] = { Vector3::Zero };
    float arc = DirectX::XM_PI * 2 / n;
    Vertex* p = &vertices[0];

    float sinValue = sinf(angle);
    float cosValue = cosf(angle);
    for (int i = 0; i <= n; i++)
    {
        float rx, ry;
        rx = cosf(arc * -i) * radius * scale.x;
        ry = sinf(arc * -i) * radius * scale.y;
        p->position.x = center.x + rx * cosValue - ry * sinValue;
        p->position.y = center.y + rx * sinValue + ry * cosValue;

        p->position.x = -1.0f + p->position.x * 2 / viewport.Width;
        p->position.y = 1.0f - p->position.y * 2 / viewport.Height;
        p->color = color;
        p++;

        p->position.x = center.x;
        p->position.y = center.y;

        p->position.x = -1.0f + p->position.x * 2 / viewport.Width;
        p->position.y = 1.0f - p->position.y * 2 / viewport.Height;
        p->color = color;
        p++;
    }

    D3D11_MAPPED_SUBRESOURCE msr;
    context->Map(_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, vertices, sizeof(Vertex) * (n + 1) * 2);
    context->Unmap(_vertexBuffer.Get(), 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->IASetInputLayout(_inputLayout.Get());
    context->VSSetShader(_vertexShader.Get(), NULL, 0);
    context->PSSetShader(_pixelShader.Get(), NULL, 0);

    context->Draw((n + 1) * 2 - 1, 0);
}

void Primitive::Capsule(ID3D11DeviceContext* context, 
    const Vector2& from, const Vector2& to, 
    const Vector2& scale, 
    const float& radius,
    const Vector4& color)
{
    Primitive::Circle(context, from, radius, scale, 0.0f, color);
    Primitive::Circle(context, to, radius, scale, 0.0f, color);
    Primitive::Line(context, from, to, color, scale.x * radius * 2.0f);
}
