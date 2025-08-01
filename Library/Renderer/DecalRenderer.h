#pragma once

#include <unordered_map>

#include "../../Library/Decal/Decal.h"
#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Graphics/GBuffer.h"
#include "../../Library/2D/Sprite.h"

class DecalRenderer
{
public:
	// 描画情報
	struct DrawInfo
	{ 
		// 使用するシェーダー名
		std::string					shaderName = "";
		// デカールの色テクスチャ
		ID3D11ShaderResourceView**	colorSRV = nullptr;
		// デカールの法線マップ
		ID3D11ShaderResourceView**	normalSRV = nullptr;
		// ワールド行列
		const DirectX::XMFLOAT4X4*	world = nullptr;
		// 色
		Vector4						color = Vector4::White;
	};
	// デカールの定数バッファ
	struct DecalConstants
	{
		DirectX::XMFLOAT4X4 world{};
		DirectX::XMFLOAT4X4 inverseTransform{};
		Vector4				color = Vector4::White;
		Vector4				direction{};
	};
	// ジオメトリの頂点データ
	struct GeometryVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};
	// ピクセルシェーダデータ
	struct PixelShaderData
	{
		Microsoft::WRL::ComPtr<ID3D11PixelShader> geometryShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> spriteShader;
	};

#pragma region シェーダー側に送る時の番号
	static constexpr UINT DECAL_COLOR_SRV_INDEX = 0;
	static constexpr UINT DECAL_NORMAL_SRV_INDEX = 1;
	static constexpr UINT GBUFFER_COLOR_SRV_INDEX = 2;
	static constexpr UINT GBUFFER_DEPTH_SRV_INDEX = 3;
	static constexpr UINT DECAL_CONSTANT_INDEX = 2;
#pragma endregion

	// ジオメトリの半辺長
	static constexpr float GEOMETRY_BOX_RADII = 0.5f;

public:
	DecalRenderer() = default;
	~DecalRenderer() = default;

	// 初期設定
	void Initialize(ID3D11Device* device, UINT width, UINT height);
	// 描画申請
	void Draw(std::string shaderName, Decal* decal, const DirectX::XMFLOAT4X4& world);
	// 描画申請
	void Draw(
		std::string shaderName,
		ID3D11ShaderResourceView** colorSRV,
		ID3D11ShaderResourceView** normalSRV,
		const DirectX::XMFLOAT4X4& world,
		const Vector4& color = Vector4::White);
	// 描画処理
	void Render(GBuffer* gbuffer, ID3D11Device* device, const RenderContext& rc);
	// シェーダ名の取得
	std::vector<const char*> GetShaderNames() const;
private:
	// 描画用COMオブジェクトの生成
	void CreateCubeCOMObject(ID3D11Device* device);

	// ピクセルシェーダ作成
	void CreatePixelShader(ID3D11Device* device, const std::string& shaderName,
		const std::string& geometryShaderFile, const std::string& spriteShaderFile);

	// ジオメトリの描画
	void DrawGeometry(ID3D11DeviceContext* dc);
private:
#pragma region 描画用COMオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer>			_geometryVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			_geometryIndexBuffer;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11Buffer>			_decalConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		_geometryVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		_geometryInputLayout;

	std::unordered_map<std::string, PixelShaderData> _geometryPixelShaders;
#pragma endregion
	// 描画情報のリスト
	std::vector<DrawInfo>							_drawInfos;
	std::unique_ptr<SpriteResource>					_fullscreenQuad;
	// デカールの法線マップがない場合のダミー
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _dummyNormalMap;
};