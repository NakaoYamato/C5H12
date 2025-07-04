#pragma once

#include "TerrainController.h"

class TerrainDeformer : public Component
{
public:
	// ブラシの種類
	enum class BrushMode
	{
		Add,        // 色加算
		Subtract,   // 色減算
		Height,     // 高さ変形
		Cost,       // コスト変形
	};
    // ブラシの定数バッファ
    struct ConstantBuffer
    {
        DirectX::XMFLOAT2 brushPosition = {}; // ブラシ位置
        float brushRadius               = 0.1f; // ブラシ半径
        float brushStrength             = 1.0f; // ブラシ強度

        float textureTillingScale       = 3.0f; // タイリング係数(基本変えない)
		DirectX::XMFLOAT2 heightScale = { -1.0f, 1.0f }; // 高さ変形スケール x : 最小値、 y : 最大値
        float brushPadding[1]{};
    };
	// 編集タスク
    struct Task
    {
		BrushMode   mode            = BrushMode::Add;   // ブラシモード
		size_t      textureIndex    = 0;                // 使用するテクスチャインデックス
		Vector2     brushUVPosition = Vector2::Zero;    // ブラシUV位置
		float       radius          = 0.0f;             // ブラシ半径
		float       strength        = 0.0f;             // ブラシ強度
		Vector4     brushColor      = Vector4::White;   // ブラシ色
		Vector2     heightScale     = { -1.0f, 1.0f };  // 高さ変形スケール x : 最小値、 y : 最大値
    };
    // 書き込むテクスチャデータ
	struct TextureData
	{
		std::wstring baseColorPath; // ベースカラーのパス
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> baseColorSRV; // ベースカラーのSRV
		std::wstring normalPath;     // 法線マップのパス
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV; // 法線マップのSRV
	};

public:
    TerrainDeformer() = default;
    ~TerrainDeformer() override = default;
    // 名前取得
    const char* GetName() const override { return "TerrainDeformer"; }
    // 生成時処理
    void OnCreate() override;
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
    // 書き込みテクスチャの読み込み
	void LoadTextureData(const std::wstring& baseColorPath, const std::wstring& normalPath);

private:
    // 地形コントローラーへの参照
    std::weak_ptr<TerrainController> _terrainController;
    // 書き込むテクスチャデータ
	std::vector<TextureData> _textureDatas;
    // 使用するブラシ
	BrushMode _brushMode = BrushMode::Add;
    // 使用するテクスチャインデックス
	size_t _textureIndex = 0;
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

    // マテリアルマップのコピーピクセルシェーダ
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _copyMaterialPS;
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

    // マテリアルマップのコピーバッファ
    std::unique_ptr<FrameBuffer> _copyMaterialMapFB;
    // テレインのパラメータマップを別枠で格納するフレームバッファ
    std::unique_ptr<FrameBuffer> _copyParameterMapFB;
};