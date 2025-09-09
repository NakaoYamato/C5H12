#pragma once

#include <unordered_map>

#include "TerrainController.h"
#include "../../Library/2D/SpriteResource.h"

// 先行宣言
class TerrainDeformerBrush;

class TerrainDeformer : public Component
{
public:
    // ブラシの定数バッファ
    struct ConstantBuffer
    {
        Vector2 brushPosition = {}; // ブラシ位置
        float brushRadius               = 0.1f; // ブラシ半径
        float brushStrength             = 1.0f; // ブラシ強度

        float textureTillingScale       = 5.0f; // タイリング係数(基本変えない)
        float brushRotationY            = 0.0f; // ブラシのY軸回転(ラジアン)
        Vector2 padding                 = {};   // パディング
    };
	// 編集タスク
    struct Task
    {
		std::string brushName       = ""; // ブラシ名
		size_t      paintTextureIndex    = 0;           // 使用するテクスチャインデックス
        size_t      brushTextureIndex = 0;              // 使用するブラシテクスチャインデックス
		Vector2     brushUVPosition = Vector2::Zero;    // ブラシUV位置
		float       radius          = 0.0f;             // ブラシ半径
		float       strength        = 0.0f;             // ブラシ強度
        float       brushRotationY  = 0.0f;             // ブラシのY軸回転(ラジアン)
        Vector2     padding         = {};               // パディング
    };
    // 書き込むテクスチャデータ
	struct PaintTexture
	{
		std::wstring baseColorPath; // ベースカラーのパス
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> baseColorSRV; // ベースカラーのSRV
		std::wstring normalPath;     // 法線マップのパス
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV; // 法線マップのSRV
		std::wstring heightPath;     // 高さマップのパス
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> heightSRV; // 高さマップのSRV
	};
    // ブラシテクスチャデータ
    struct BrushTexture
    {
        std::wstring path;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
    };
    // 配置するモデルデータ
    struct ModelData
    {
        std::string path = ""; // モデルのパス
    };

    static constexpr size_t PaintBaseColorTextureIndex = 4;
    static constexpr size_t PaintNormalTextureIndex = 5;
    static constexpr size_t PaintHeightTextureIndex = 6;
    static constexpr size_t BrushTextureIndex = 7;
public:
    TerrainDeformer() = default;
    ~TerrainDeformer() override = default;
    // 名前取得
    const char* GetName() const override { return "TerrainDeformer"; }
    // 生成時処理
    void OnCreate() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // 描画処理
    void Render(const RenderContext& rc) override;
    // GUI描画
    void DrawGui() override;

    // 編集タスクの追加
	void AddTask(TerrainController* controller, const Task& task)
	{
		_taskMap[controller].push_back(task);
	}
    // 環境物を追加
    void AddEnvironmentObject(TerrainController* controller,
        const std::string& modelPath,
        TerrainObjectLayout::UpdateType updateType,
        TerrainObjectLayout::CollisionType collisionType,
        const Vector3& position,
        const Vector3& rotation,
        const Vector3& size,
        const Vector3& collisionOffset,
        const Vector4& collisionParameter);

	// ブラシの追加
    void RegisterBrush(std::shared_ptr<TerrainDeformerBrush> brush);

	// 選択中のペイントテクスチャ番号取得
	size_t GetPaintTextureIndex() const { return _paintTextureIndex; }
    // 選択中のブラシテクスチャ番号取得
	size_t GetBrushTextureIndex() const { return _brushTextureIndex; }
	// 選択中のモデルファイルパス取得
	const std::string& GetSelectedModelPath() const { return _selectedModelPath; }

private:
    // テクスチャ読み込み
    void LoadTexture(const std::wstring& path, ID3D11ShaderResourceView** srv);
    // ペイントテクスチャの追加
    void AddPaintTexture(const std::wstring& baseColorPath, const std::wstring& normalPath, const std::wstring& heightPath);
    // ブラシテクスチャの追加
    void AddBrushTexture(const std::wstring& path);
    // 配置するモデルデータの追加
	void AddModelData(const std::string& modelPath);

    // ブラシの描画
    void DrawBrush();

    // ペイントテクスチャのGUI描画
    void DrawPaintTextureGui();
    // ブラシテクスチャのGUI描画
    void DrawBrushTextureGui();
    // モデルの選択GUI描画
	void DrawModelSelectionGui();
	// ブラシの選択GUI描画
	void DrawBrushSelectionGui();
    // ブラシのGUI描画
    void DrawBrushGui();
private:
    // マテリアルマップのコピーピクセルシェーダ
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _copyMaterialPS;
    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> _constantBuffer;
    // マテリアルマップのコピーバッファ
    std::unique_ptr<FrameBuffer> _copyMaterialMapFB;
	// フルスクリーンクアッドのスプライトリソース
    std::unique_ptr<SpriteResource>		_fullscreenQuad;

    // ペイントテクスチャデータ
	std::vector<PaintTexture> _paintTextures;
    // ブラシのテクスチャデータ
    std::vector<BrushTexture> _brushTextures;
	// 配置するモデルデータ
	std::vector<ModelData> _environmentObjects;
    // 編集タスク群
	std::unordered_map<TerrainController*, std::vector<Task>> _taskMap;

    // 使用するテクスチャインデックス
	size_t _paintTextureIndex = 0;
    // 使用するブラシテクスチャインデックス
    size_t _brushTextureIndex = 0;
    // 選択中のモデルファイルパス
    std::string _selectedModelPath = "";

    // ブラシの行列
    DirectX::XMFLOAT4X4 _brushMatrix = {};
    // レイキャストの長さ
    float _rayLength = 1000.0f;
    // ブラシ使用フラグ
    bool _useBrush = false;
    // 前フレームのGUI操作フラグ
    bool _wasGuiActive = false;
	// タイリング係数
	float _textureTillingScale = 5.0f;

    // 登録しているブラシ
	std::unordered_map<std::string, std::shared_ptr<TerrainDeformerBrush>> _brushes;
	// 選択中のブラシ名
	std::string _selectedBrushName = "";
};

class TerrainDeformerBrush
{
public:
    TerrainDeformerBrush(TerrainDeformer* deformer) :
        _deformer(deformer)
    {
    }
    virtual ~TerrainDeformerBrush() {}

	// 名前取得
	virtual const char* GetName() const = 0;
	// 更新処理
    virtual void Update(std::vector<std::shared_ptr<TerrainController>>& terrainControllers,
        float elapsedTime,
        Vector3* intersectWorldPosition);
	// 描画処理
    virtual void Render(SpriteResource* fullscreenQuad,
        std::shared_ptr<Terrain> terrain,
        const RenderContext& rc,
        ID3D11ShaderResourceView** srv,
        uint32_t startSlot,
        uint32_t numViews);
	// GUI描画
    virtual void DrawGui();

	// タスクを登録
    virtual void RegisterTask(std::weak_ptr<TerrainController> terrainController,
        const Vector2& uvPosition, 
        float radius,
        float strength);
#pragma region アクセサ
	// ブラシの位置取得
	const Vector2& GetBrushPosition() const { return _brushPosition; }
	// ブラシの半径取得
	float GetBrushRadius() const { return _brushRadius; }
	// ブラシの強度取得
	float GetBrushStrength() const { return _brushStrength; }
	// ブラシのY軸回転(ラジアン)取得
	float GetBrushRotationY() const { return _brushRotationY; }
	// パディング取得
	const Vector2& GetBrushPadding() const { return _brushPadding; }
	// ブラシのデバッグ描画フラグ取得
	bool IsDrawDebugBrush() const { return _drawDebugBrush; }
#pragma endregion

protected:
    // 地形変形者への参照
    TerrainDeformer* _deformer;
    // ピクセルシェーダ
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;

    // ブラシの位置
    Vector2 _brushPosition = Vector2::Zero;
    // ブラシの半径
    float _brushRadius = 2.0f;
    // ブラシの強度
    float _brushStrength = 10.0f;
    // ブラシのY軸回転(ラジアン)
    float _brushRotationY = 0.0f;
    // パディング
    Vector2 _brushPadding = {};
    // ブラシのデバッグ描画フラグ
	bool _drawDebugBrush = true;
};
