#pragma once

#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include <string>
#include <unordered_map>

#include "../../Library/Math/Vector.h"
#include "../../Library/Model/Model.h"
#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/PostProcess/FrameBuffer.h"
#include "../../Library/Exporter/Exporter.h"

class Terrain
{
public:
	// 頂点データ構造体
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
    };
	// 定数バッファのデータ構造体
    struct ConstantBuffer
    {
        DirectX::XMFLOAT4X4 world = {};

        float edgeFactor    = 64.0f;    // エッジ分割数
        float innerFactor   = 64.0f;    // 内部部分数
        float heightScaler  = 1.0f;     // 高さ係数
        float padding0      = 0.0f;     // パディング

        float emissive      = 0.0f;     // エミッシブ
        float metalness     = 0.63f;    // メタリック
        float roughness     = 0.6f;     // ラフネス
        float padding1      = 0.0f;     // パディング
    };
    // ストリームアウト用
    struct StreamOutVertex
    {
        DirectX::XMFLOAT4 position = {};
        DirectX::XMFLOAT3 worldPosition = {};
        DirectX::XMFLOAT3 normal = {};
        DirectX::XMFLOAT2 texcoord = {};
		float             cost = 0.0f;
    };
    // 透明壁構造体
	struct TransparentWall
	{
		// 透明壁の下部分の頂点
		std::vector<Vector3> points;
		// 透明壁の高さ
		float height = 5.0f;

        // 書き出し
        void Export(const char* label, nlohmann::json* jsonData);
        // 読み込み
        void Inport(const char* label, nlohmann::json& jsonData);
	};
    // 環境オブジェクトデータ
	struct EnvironmentObject
	{
        // モデル
        std::unique_ptr<Model> model;
		// モデルのパス
		std::string modelPath = "";
        // 位置
		Vector3 position = Vector3::Zero;
        // 回転
		Vector3 rotation = Vector3::Zero;
        // スケール
		Vector3 scale = Vector3::One;
	};

	static constexpr size_t BaseColorTextureIndex = 0;
	static constexpr size_t NormalTextureIndex = 1;
    static constexpr LONG StreamOutMaxVertex = 3 * 3 * 64 * 64;
    static constexpr LONG MaterialMapSize = 2048;
    static constexpr LONG ParameterMapSize = 1024;
    static constexpr UINT ParameterMapIndex = 6;
public:
    Terrain(ID3D11Device* device);
	~Terrain() {}
    // 描画処理
    void Render(const RenderContext& rc, const DirectX::XMFLOAT4X4& world, bool writeGBuffer);
    // GUI描画
    void DrawGui();

    // レイキャスト
	bool Raycast(
        const DirectX::XMFLOAT4X4& world,
        const Vector3& rayStart,
		const Vector3& rayDirection,
		float rayLength,
		Vector3* intersectionWorldPoint = nullptr,
		Vector3* intersectionWorldNormal = nullptr,
		Vector2* intersectUVPosition = nullptr) const;
#pragma region アクセサ
    // マテリアルマップのフレームバッファを取得
	FrameBuffer* GetMaterialMapFB() { return _materialMapFB.get(); }
    // パラメータマップのフレームバッファを取得
    FrameBuffer* GetParameterMapFB() { return _parameterMapFB.get(); }
    // ストリームアウトデータを取得
    const std::vector<StreamOutVertex>& GetStreamOutData() const { return _streamOutData; }
    // ストリームアウトをするかどうか設定
    void SetStreamOut(bool streamOut) { _streamOut = streamOut; }
    // 透明壁取得
	std::vector<TransparentWall>& GetTransparentWalls() { return _transparentWalls; }
    // 透明壁追加
	void AddTransparentWall(const TransparentWall& wall) { _transparentWalls.push_back(wall); }
	// 環境オブジェクトの配置情報を取得
	std::unordered_map<int, EnvironmentObject>& GetEnvironmentObjects() { return _environmentObjects; }
    // 環境オブジェクトの配置情報を追加
    void AddEnvironmentObject(ID3D11Device* device, const char* filename, Vector3 position, Vector3 rotation, Vector3 size);
	// 基本色テクスチャのパスを取得
	const std::wstring& GetBaseColorTexturePath() const { return _baseColorTexturePath; }
	// 法線テクスチャのパスを取得
	const std::wstring& GetNormalTexturePath() const { return _normalTexturePath; }
	// パラメータテクスチャのパスを取得
	const std::wstring& GetParameterTexturePath() const { return _parameterTexturePath; }
	// 基本色テクスチャのパスを設定
	void SetBaseColorTexturePath(const std::wstring& path) { _baseColorTexturePath = path; }
	// 法線テクスチャのパスを設定
	void SetNormalTexturePath(const std::wstring& path) { _normalTexturePath = path; }
	// パラメータテクスチャのパスを設定
	void SetParameterTexturePath(const std::wstring& path) { _parameterTexturePath = path; }
    // 基本色テクスチャのの書き出し
	void SaveBaseColorTexture(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* baseColorPath);
	// 法線テクスチャの書き出し
	void SaveNormalTexture(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* normalPath);
    // パラメータマップの書き出し
    void SaveParameterMap(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* heightMapPath);

    // 書き出し
    void SaveToFile(const std::string& path);
    // 読み込み
    void LoadFromFile(const std::string& path);
#pragma endregion

private:
    // 地形メッシュの頂点とインデックスを生成
    void CreateTerrainMesh(ID3D11Device* device);
private:
    ConstantBuffer                          _data;
#pragma region 描画用COMオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer>	_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>    _indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>    _constantBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>	_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>	_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11HullShader>	_hullShader;
    Microsoft::WRL::ComPtr<ID3D11DomainShader>	_domainShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	_gbPixelShader;

    // ストリームアウト用
    Microsoft::WRL::ComPtr<ID3D11Buffer> _streamOutVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _streamOutCopyBuffer;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>	_streamOutGeometryShader;
#pragma endregion

    // マテリアルマップ(RT0:BaseColor,RT1:Normal)
    std::unique_ptr<FrameBuffer> _materialMapFB;
    // パラメータマップ（R：高さ、G：コスト、B：（空き）、A：（空き））
    std::unique_ptr<FrameBuffer> _parameterMapFB;
    // マテリアルのリセット
    bool _resetMap = false;

    // ストリームアウト用
    bool _streamOut = false;
    std::vector<StreamOutVertex> _streamOutData;

	// 基本色テクスチャのパス
    std::wstring _baseColorTexturePath = L"";
	// 法線テクスチャのパス
    std::wstring _normalTexturePath = L"";
	// パラメータテクスチャのパス
    std::wstring _parameterTexturePath = L"";

    // 透明壁
    std::vector<TransparentWall> _transparentWalls;

    // 環境オブジェクトの配置情報
	std::unordered_map<int, EnvironmentObject> _environmentObjects;
    // 現在の環境オブジェクトの配置番号
	int _currentEnvironmentObjectIndex = 0;
};