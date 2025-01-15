#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "../../Library/Math/Vector.h"

class PrimitiveRenderer
{
public:
	PrimitiveRenderer(ID3D11Device* device);

	// í∏ì_í«â¡
	void AddVertex(const Vector3& position, const Vector4& color);

	// ï`âÊé¿çs
	void Render(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

private:
	static const UINT VertexCapacity = 3 * 1024;

	struct CbScene
	{
		DirectX::XMFLOAT4X4		viewProjection;
		Vector4		color;
	};

	struct Vertex
	{
		Vector3	position;
		Vector4	color;
	};
	std::vector<Vertex>		vertices;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer;
};