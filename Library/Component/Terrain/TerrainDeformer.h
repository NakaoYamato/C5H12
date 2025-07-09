#pragma once

#include "TerrainController.h"

// 先行宣言
class TerrainDeformerBrush;

class TerrainDeformer : public Component
{
public:
    // ブラシの定数バッファ
    struct ConstantBuffer
    {
        DirectX::XMFLOAT2 brushPosition = {}; // ブラシ位置
        float brushRadius               = 0.1f; // ブラシ半径
        float brushStrength             = 1.0f; // ブラシ強度

        float textureTillingScale       = 3.0f; // タイリング係数(基本変えない)
        float brushRotationY            = 0.0f; // ブラシのY軸回転(ラジアン)
		DirectX::XMFLOAT2 heightScale = { -1.0f, 1.0f }; // 高さ変形スケール x : 最小値、 y : 最大値
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
		Vector2     heightScale     = { -1.0f, 1.0f };  // 高さ変形スケール x : 最小値、 y : 最大値
    };
    // 書き込むテクスチャデータ
	struct PaintTexture
	{
		std::wstring baseColorPath; // ベースカラーのパス
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> baseColorSRV; // ベースカラーのSRV
		std::wstring normalPath;     // 法線マップのパス
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV; // 法線マップのSRV
	};
    // ブラシテクスチャデータ
    struct BrushTexture
    {
        std::wstring path;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
    };

    static constexpr size_t PaintBaseColorTextureIndex = 3;
    static constexpr size_t PaintNormalTextureIndex = 4;
    static constexpr size_t BrushTextureIndex = 5;
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

	// ブラシの追加
    void RegisterBrush(std::shared_ptr<TerrainDeformerBrush> brush);

	// 選択中のペイントテクスチャ番号取得
	size_t GetPaintTextureIndex() const { return _paintTextureIndex; }
    // 選択中のブラシテクスチャ番号取得
	size_t GetBrushTextureIndex() const { return _brushTextureIndex; }

private:
    // テクスチャ読み込み
    void LoadTexture(const std::wstring& path, ID3D11ShaderResourceView** srv);
    // ペイントテクスチャの追加
    void AddPaintTexture(const std::wstring& baseColorPath, const std::wstring& normalPath);
    // ブラシテクスチャの追加
    void AddBrushTexture(const std::wstring& path);
    // SRVを表示して、クリックされたらパスを更新するダイアログを開く
    bool ShowAndEditImage(std::wstring* path, ID3D11ShaderResourceView* srv);

    // ペイントテクスチャのGUI描画
    void DrawPaintTextureGui();
    // ブラシテクスチャのGUI描画
    void DrawBrushTextureGui();
    // モデルの選択GUI描画
	void DrawModelSelectionGui();
	// ブラシの選択GUI描画
	void DrawBrushSelectionGui();
private:
    // 地形コントローラーへの参照
    std::weak_ptr<TerrainController> _terrainController;
    // マテリアルマップのコピーピクセルシェーダ
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _copyMaterialPS;
    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> _constantBuffer;
    // マテリアルマップのコピーバッファ
    std::unique_ptr<FrameBuffer> _copyMaterialMapFB;
    // テレインのパラメータマップを別枠で格納するフレームバッファ
    std::unique_ptr<FrameBuffer> _copyParameterMapFB;

    // 書き込むテクスチャデータ
	std::vector<PaintTexture> _paintTextures;
    // ブラシのテクスチャデータ
    std::vector<BrushTexture> _brushTextures;
    // 編集タスク群
    std::vector<Task> _tasks;

    // 使用するテクスチャインデックス
	size_t _paintTextureIndex = 0;
    // 使用するブラシテクスチャインデックス
    size_t _brushTextureIndex = 0;

    // ブラシ使用フラグ
    bool _useBrush = false;
    // 前フレームのGUI操作フラグ
    bool _wasGuiActive = false;

    // 選択中のモデルファイルパス
	std::string _selectedModelPath = "";

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
    virtual void Update(std::shared_ptr<Terrain> terrain, float elapsedTime);
	// 描画処理
    virtual void Render(std::shared_ptr<Terrain> terrain,
        const RenderContext& rc,
        ID3D11ShaderResourceView** srv,
        uint32_t startSlot,
        uint32_t numViews) {};
	// GUI描画
    virtual void DrawGui(std::shared_ptr<Terrain> terrain);

	// タスクを登録
    virtual void RegisterTask(const Vector2& uvPosition, float radius, float strength);
#pragma region アクセサ
	// ブラシの位置(ワールド)取得
	const Vector3& GetBrushWorldPosition() const { return _brushWorldPosition; }
	// ブラシの位置取得
	const Vector2& GetBrushPosition() const { return _brushPosition; }
	// ブラシの半径取得
	float GetBrushRadius() const { return _brushRadius; }
	// ブラシの強度取得
	float GetBrushStrength() const { return _brushStrength; }
	// ブラシのY軸回転(ラジアン)取得
	float GetBrushRotationY() const { return _brushRotationY; }
	// 高さ変形スケール取得 x : 最小値、 y : 最大値
	const Vector2& GetBrushHeightScale() const { return _brushHeightScale; }
	// ブラシのデバッグ描画フラグ取得
	bool IsDrawDebugBrush() const { return _drawDebugBrush; }
#pragma endregion

protected:
    // 地形変形者への参照
    TerrainDeformer* _deformer;
    // ピクセルシェーダ
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;

    // レイキャストの長さ
    float _rayLength = 1000.0f;
    // ブラシの位置(ワールド)
    Vector3 _brushWorldPosition = Vector3::Zero;
    // ブラシの位置
    Vector2 _brushPosition = Vector2::Zero;
    // ブラシの半径
    float _brushRadius = 2.0f;
    // ブラシの強度
    float _brushStrength = 10.0f;
    // ブラシのY軸回転(ラジアン)
    float _brushRotationY = 0.0f;
    // 高さ変形スケール x : 最小値、 y : 最大値
    Vector2 _brushHeightScale = { -100.0f, 100.0f };
    // ブラシのデバッグ描画フラグ
	bool _drawDebugBrush = true;
};
