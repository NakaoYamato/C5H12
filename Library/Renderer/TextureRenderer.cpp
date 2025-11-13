#include "TextureRenderer.h"

#include "../Graphics/GpuResourceManager.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../HRTrace.h"

#include "../../Library/Shader/Sprite/SpriteShaderResource.h"

/// 初期化
void TextureRenderer::Initialize(ID3D11Device* device)
{
	// リソースを初期化
	auto spriteShaderResource = ResourceManager::Instance().GetResourceAs<SpriteShaderResource>();
	spriteShaderResource->Initialize(device);

	// 頂点情報のセット
	Vertex vertices[]
	{
		{{0.0f,0.0f,0}, Vector4::White ,Vector2::Zero		},
		{{0.0f,0.0f,0}, Vector4::White ,Vector2::XOneYZero},
		{{0.0f,0.0f,0}, Vector4::White ,Vector2::XZeroYOne},
		{{0.0f,0.0f,0}, Vector4::White ,Vector2::One		},
	};

	// 頂点バッファオブジェクトの生成
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	HRESULT hr{ S_OK };
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, &_vertexBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void TextureRenderer::Render(
	const RenderContext& rc,
	ID3D11ShaderResourceView* const* srv,
	const D3D11_TEXTURE2D_DESC& textureDesc,
	const Vector2& position,
	const Vector2& scale,
	const Vector2& texPos,
	const Vector2& texSize,
	const Vector2& center,
	float angle,
	const Vector4& color,
	Material* material)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// スケールが0なら描画する必要がない
	if (scale.x == 0.0f || scale.y == 0.0f) return;

	// スクリーンのサイズ取得
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 引数から各頂点の位置の計算
	// (x0, y0) *----* (x2, y2)
	//			|	/|
	//			|  / |
	//			| /	 |
	//			|/   |
	// (x1, y1) *----* (x3, y3) 

	// 切り取りサイズ判定
	float tw = texSize.x;
	float th = texSize.y;
	if (texSize.x == 0.0f && texSize.y == 0.0f)
	{
		tw = static_cast<float>(textureDesc.Width);
		th = static_cast<float>(textureDesc.Height);
	}

	Vertex vertices[4] = {};
	vertices[0] = { Vector3(0.0f, 0.0f, 0), color, Vector2::Zero };
	vertices[1] = { Vector3(0.0f, 1.0f, 0), color, Vector2::XZeroYOne };
	vertices[2] = { Vector3(1.0f, 0.0f, 0), color, Vector2::XOneYZero };
	vertices[3] = { Vector3(1.0f, 1.0f, 0), color, Vector2::One };

	const float sinValue = sinf(angle);
	const float cosValue = cosf(angle);
	const float mx = scale.x * center.x;
	const float my = scale.y * center.y;
	for (int i = 0; i < 4; i++)
	{
		vertices[i].position.x *= (tw * scale.x);
		vertices[i].position.y *= (th * scale.y);

		vertices[i].position.x -= mx;
		vertices[i].position.y -= my;

		const float rx = vertices[i].position.x;
		const float ry = vertices[i].position.y;
		vertices[i].position.x = rx * cosValue - ry * sinValue;
		vertices[i].position.y = rx * sinValue + ry * cosValue;

		vertices[i].position.x += mx;
		vertices[i].position.y += my;

		vertices[i].position.x += (position.x - scale.x * center.x);
		vertices[i].position.y += (position.y - scale.y * center.y);

		vertices[i].position.x = -1.0f + vertices[i].position.x * 2 / viewport.Width;
		vertices[i].position.y = 1.0f - vertices[i].position.y * 2 / viewport.Height;
		const float UV_ADJUST = 1.0f;// 0.99994f;
		// UV座標の調整
		vertices[i].texcoord.x = (std::min)(vertices[i].texcoord.x, UV_ADJUST);
		vertices[i].texcoord.y = (std::min)(vertices[i].texcoord.y, UV_ADJUST);

		vertices[i].texcoord.x = (texPos.x + vertices[i].texcoord.x * tw) / textureDesc.Width;
		vertices[i].texcoord.y = (texPos.y + vertices[i].texcoord.y * th) / textureDesc.Height;
	}

	D3D11_MAPPED_SUBRESOURCE msr;
	dc->Map(_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertices, sizeof(vertices));
	dc->Unmap(_vertexBuffer.Get(), 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	auto spriteShaderResource = ResourceManager::Instance().GetResourceAs<SpriteShaderResource>();
	// シェーダ取得
	auto* shader = material != nullptr ?
		spriteShaderResource->GetShader(material->GetShaderName()) :
		spriteShaderResource->GetShader("Simple");
	// シェーダが見つからなかった場合はSimpleシェーダーを使用
	if (shader == nullptr)
	{
		Debug::Output::String(L"\tTextureRenderer\n");
		Debug::Output::String(L"\tシェーダーがnullptrのためSimpleシェーダーを使用\n");
		shader = spriteShaderResource->GetShader("Simple");
	}
	shader->Begin(rc);

	dc->PSSetShaderResources(0, 1, srv);

	shader->Update(rc, material);

	dc->Draw(4, 0);

	shader->End(rc);
}

void TextureRenderer::Render(
	const RenderContext& rc,
	Texture* texture, 
	const Vector2& position,
	const Vector2& scale,
	const Vector2& texPos,
	const Vector2& texSize,
	const Vector2& center,
	float angle,
	const Vector4& color,
	Material* material)
{
	Render(rc,
		texture->GetAddressOf(),
		texture->GetTexture2DDesc(),
		position,
		scale,
		texPos,
		texSize,
		center,
		angle,
		color,
		material);
}

void TextureRenderer::Blit(
	ID3D11DeviceContext* dc,
	ID3D11ShaderResourceView** srv,
	uint32_t startSlot, 
	uint32_t numViews, 
	ID3D11PixelShader* pixelShader)
{
	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(nullptr);

	auto spriteShaderResource = ResourceManager::Instance().GetResourceAs<SpriteShaderResource>();

	dc->VSSetShader(spriteShaderResource->GetBlitVS(), nullptr, 0);
	pixelShader != nullptr ?
		dc->PSSetShader(pixelShader, nullptr, 0) :
		dc->PSSetShader(spriteShaderResource->GetBlitPS(), nullptr, 0);

	// シェーダーリソースビューの設定
	if (numViews != 0)
		dc->PSSetShaderResources(startSlot, numViews, srv);

	dc->Draw(4, 0);

	// シェーダーリソースビューのクリア
	ID3D11ShaderResourceView* srvs[] = { nullptr };
	for (uint32_t i = 0; i < numViews; ++i)
	{
		dc->PSSetShaderResources(startSlot + i, 1, srvs);
	}
	dc->VSSetShader(nullptr, nullptr, 0);
}
