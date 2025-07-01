#pragma once

#include "TerrainController.h"

class TerrainDeformer : public Component
{
public:
	enum class BrushMode
	{
		Add,        // 色加算
		Subtract,   // 色減算
		Height,     // 高さ変形
	};

    struct ConstantBuffer
    {
        DirectX::XMFLOAT2 brushPosition = {}; // ブラシ位置
        float brushRadius               = 0.1f; // ブラシ半径
        float brushStrength             = 1.0f; // ブラシ強度

        DirectX::XMFLOAT4 brushColor    = Vector4::White; // ブラシ色

		DirectX::XMFLOAT2 heightScale = { -1.0f, 1.0f }; // 高さ変形スケール x : 最小値、 y : 最大値
        float brushPadding[2]{};
    };

public:
    TerrainDeformer() = default;
    ~TerrainDeformer() override = default;
    // 名前取得
    const char* GetName() const override { return "TerrainDeformer"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // 描画処理
    void Render(const RenderContext& rc) override;
    // GUI描画
    void DrawGui() override;

private:
    // 地形コントローラーへの参照
    std::weak_ptr<TerrainController> _terrainController;
    // 使用するブラシ
	BrushMode _brushMode = BrushMode::Add;

    Vector3 _intersectionWorldPoint = Vector3::Zero; // 交差点
    float brushRadius = 0.1f; // ブラシ半径
    float brushStrength = 1.0f; // ブラシ強度
    bool _useBrush = false; // ブラシ使用フラグ
    bool _isIntersect = false; // 交差したかどうか
    bool _isDeforming = false; // 変形中フラグ

    // 加算ブラシピクセルシェーダ
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _addBrushPS;
	// 減算ブラシピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _subtractBrushPS;
	// 高さ変形ブラシピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _heightBrushPS;
    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> _constantBuffer;
    // 定数バッファデータ
    ConstantBuffer _constantBufferData{};

    // テレインのパラメータマップを別枠で格納するフレームバッファ
    std::unique_ptr<FrameBuffer> _parameterMapFB;
};