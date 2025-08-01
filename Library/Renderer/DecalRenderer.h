#pragma once

#include "../../Library/Decal/Decal.h"
#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Graphics/GBuffer.h"
#include "../../Library/2D/Sprite.h"

class DecalRenderer
{
public:
	struct DrawInfo
	{
		Decal* decal = nullptr; // 描画するデカール
		const DirectX::XMFLOAT4X4* world = nullptr; // ワールド行列
		const Vector4* color = nullptr; // 色
	};
	struct DecalConstants
	{
		DirectX::XMFLOAT4X4 world{};
		DirectX::XMFLOAT4X4 inverseTransform{};
		Vector4				color = Vector4::White;
		Vector4 direction{};
	};
	struct GeometryVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};
	static constexpr UINT DECAL_COLOR_SRV_INDEX = 0;
	static constexpr UINT DECAL_NORMAL_SRV_INDEX = 1;
	static constexpr UINT GBUFFER_DEPTH_SRV_INDEX = 2;
	static constexpr UINT DECAL_CONSTANT_INDEX = 2;

public:
	DecalRenderer() = default;
	~DecalRenderer() = default;

	// 初期設定
	void Initialize(ID3D11Device* device, UINT width, UINT height);
	// 描画申請
	void Draw(Decal* decal, const DirectX::XMFLOAT4X4& world, const Vector4& color);
	// 描画処理
	void Render(GBuffer* gbuffer, ID3D11Device* device, const RenderContext& rc);
private:
	// 描画用COMオブジェクトの生成
	void CreateCubeCOMObject(ID3D11Device* device);

	// ジオメトリの描画
	void DrawGeometry(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& world,
		const Vector4& color);
private:
#pragma region 描画用COMオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> _geometryVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _geometryIndexBuffer;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11Buffer> _decalConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> _geometryVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> _geometryInputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _geometryPixelShader;
#pragma endregion
	std::vector<DrawInfo> _drawInfos; // 描画情報のリスト
	std::unique_ptr<SpriteResource>			_fullscreenQuad;
};