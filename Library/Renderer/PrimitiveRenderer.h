#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "../../Library/Math/Vector.h"
#include "../../Library/Math/Quaternion.h"

#include "../../Library/Graphics/Shader.h"
#include "../../Library/Graphics/ConstantBuffer.h"

class PrimitiveRenderer
{
public:
	static const UINT VertexCapacity = 3 * 1024;

	static const UINT CBIndex = 1;
	static const UINT ColorSRVIndex = 3;
	static const UINT ParameterSRVIndex = 4;

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

	struct RenderInfo
	{
		ID3D11ShaderResourceView**	colorSRV		= nullptr;
		ID3D11ShaderResourceView**	parameterSRV	= nullptr;
		PixelShader*				pixelShader		= nullptr;
		ConstantBuffer*				constantBuffer	= nullptr;

		std::vector<Vertex>			vertices;
	};

public:
	PrimitiveRenderer() = default;
	~PrimitiveRenderer() = default;

	// 初期化処理
	void Initialize(ID3D11Device* device);

	// 描画
	void Draw(const RenderInfo& info);

	// 描画実行
	void Render(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

private:
	std::vector<RenderInfo>						_renderInfos;
	PixelShader									_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer;
};
