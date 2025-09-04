#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "../../Library/Math/Vector.h"
#include "../../Library/Math/Quaternion.h"
#include "../../Library/Shader/Shader.h"

class PrimitiveRenderer
{
public:
	PrimitiveRenderer() = default;
	~PrimitiveRenderer() = default;

	// 初期化処理
	void Initialize(ID3D11Device* device);

	// 頂点追加
	void AddVertex(const Vector3& position, const Vector4& color);

	// 描画実行
	void Render(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

private:
	static const UINT VertexCapacity = 3 * 1024;

	struct CbScene
	{
		UINT vertexCount; // 頂点数
		DirectX::XMFLOAT2 viewportSize; // ビューポートのサイズ
		float padding; // パディング用
	};

	struct Vertex
	{
		Vector3	position;
		Vector4	color;
	};
	std::vector<Vertex>		vertices;

	PixelShader			_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _noiseSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _distanceSRV;

	float timer = 0.0f; // タイマー
};
