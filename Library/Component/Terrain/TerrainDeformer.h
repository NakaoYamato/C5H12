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
		Cost,       // コスト変形
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

    struct Task
    {
		BrushMode   mode            = BrushMode::Add;   // ブラシモード
		Vector2     brushUVPosition = Vector2::Zero;    // ブラシUV位置
		float       radius          = 0.0f;             // ブラシ半径
		float       strength        = 0.0f;             // ブラシ強度
		Vector4     brushColor      = Vector4::White;   // ブラシ色
		Vector2     heightScale     = { -1.0f, 1.0f };  // 高さ変形スケール x : 最小値、 y : 最大値
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

    // 編集タスクの追加
	void AddTask(const Task& task)
	{
		_tasks.push_back(task);
	}
private:
    // 地形コントローラーへの参照
    std::weak_ptr<TerrainController> _terrainController;
    // 使用するブラシ
	BrushMode _brushMode = BrushMode::Add;
    // ブラシの色
	Vector4 _brushColor = Vector4::White;
    // 交差点
    Vector3 _intersectionWorldPoint = Vector3::Zero;
    // ブラシの高さ変形スケール x : 最小値、 y : 最大値
    Vector2     _brushHeightScale = { -1.0f, 1.0f };
    // ブラシ半径
    float brushRadius = 0.1f;
    // ブラシ強度
    float brushStrength = 1.0f;
    // ブラシ使用フラグ
    bool _useBrush = false;
    // 編集タスク群
	std::vector<Task> _tasks;

    // 加算ブラシピクセルシェーダ
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _addBrushPS;
	// 減算ブラシピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _subtractBrushPS;
	// 高さ変形ブラシピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _heightBrushPS;
	// コスト変形ブラシピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _costBrushPS;
    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> _constantBuffer;

    // テレインのパラメータマップを別枠で格納するフレームバッファ
    std::unique_ptr<FrameBuffer> _parameterMapFB;
	// テレインのデータマップを別枠で格納するフレームバッファ
	std::unique_ptr<FrameBuffer> _dataMapFB;
};