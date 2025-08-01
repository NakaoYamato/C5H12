#include "SpriteResource.h"

#include <algorithm>

#include "../HRTrace.h"
#include "../Graphics/GpuResourceManager.h"

SpriteResource::SpriteResource(ID3D11Device* device,
    const wchar_t* filename, 
    const char* vsShaderFilename, 
    const char* psShaderFilename)
{
	// filename == L""ならダミーを作る
	if (filename == L"")
	{
		GpuResourceManager::MakeDummyTexture(device,
			&_srv,
			&_texture2dDesc,
			0xFFFFFFFF, 16);
	}
	else
	{
		GpuResourceManager::LoadTextureFromFile(
			device, filename, &_srv, &_texture2dDesc);
	}

	// 頂点情報のセット
	Vertex vertices[]
	{
		{{-1.0f,+1.0f,0}, Vector4::White ,Vector2::Zero		},
		{{+1.0f,+1.0f,0}, Vector4::White ,Vector2::XOneYZero},
		{{-1.0f,-1.0f,0}, Vector4::White ,Vector2::XZeroYOne},
		{{+1.0f,-1.0f,0}, Vector4::White ,Vector2::One		},
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

	// 頂点シェーダーオブジェクトの生成
	// 入力レイアウトオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	GpuResourceManager::CreateVsFromCso(device, vsShaderFilename, &_vertexShader, &_inputLayout,
		input_element_desc, _countof(input_element_desc));

	// ピクセルシェーダーオブジェクトの生成
	GpuResourceManager::CreatePsFromCso(device, psShaderFilename, &_pixelShader);
}

SpriteResource::SpriteResource(ID3D11Device* device,
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv, 
    const char* vsShaderFilename, 
    const char* psShaderFilename)
{
	// 頂点情報のセット
	Vertex vertices[]
	{
		{{-1.0f,+1.0f,0}, Vector4::White ,Vector2::Zero		},
		{{+1.0f,+1.0f,0}, Vector4::White ,Vector2::XOneYZero},
		{{-1.0f,-1.0f,0}, Vector4::White ,Vector2::XZeroYOne},
		{{+1.0f,-1.0f,0}, Vector4::White ,Vector2::One		},
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

	// 頂点シェーダーオブジェクトの生成
	// 入力レイアウトオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	GpuResourceManager::CreateVsFromCso(device, vsShaderFilename, &_vertexShader, &_inputLayout,
		input_element_desc, _countof(input_element_desc));

	// ピクセルシェーダーオブジェクトの生成
	GpuResourceManager::CreatePsFromCso(device, psShaderFilename, &_pixelShader);

	if (srv)
	{
		_isLoadFile = false;
		srv.Get()->AddRef();
		this->_srv = srv;

		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		this->_srv->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		//texture2d
		texture2d->GetDesc(&_texture2dDesc);
	}
}

SpriteResource::~SpriteResource()
{
	if (!_isLoadFile)
		_srv->Release();
}

/// 描画処理
void SpriteResource::Render(ID3D11DeviceContext* dc, 
    const Vector2& position, 
    const Vector2& scale, 
    const Vector2& texPos,
    const Vector2& texSize,
    const Vector2& center, 
    float angle, 
    const Vector4& color) const
{
	// スケールが0なら描画する必要がない
	if (scale.x == 0.0f || scale.y == 0.0f) return;

	// スクリーンのサイズ取得
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 引数から各頂点の位置の計算
	// (x0, y0) *----* (x1, y1)
	//			|	/|
	//			|  / |
	//			| /	 |
	//			|/   |
	// (x2, y2) *----* (x3, y3) 

	// 切り取りサイズ判定
	float tw = texSize.x;
	float th = texSize.y;
	if (texSize.x == 0.0f && texSize.y == 0.0f)
	{
		tw = static_cast<float>(_texture2dDesc.Width);
		th = static_cast<float>(_texture2dDesc.Height);
	}

	Vertex vertices[4] = {};
	vertices[0] = { Vector3(0.0f, 0.0f, 0), color, Vector2::Zero		};
	vertices[1] = { Vector3(1.0f, 0.0f, 0), color, Vector2::XOneYZero	};
	vertices[2] = { Vector3(0.0f, 1.0f, 0), color, Vector2::XZeroYOne	};
	vertices[3] = { Vector3(1.0f, 1.0f, 0), color, Vector2::One			};

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

		vertices[i].texcoord.x = (texPos.x + vertices[i].texcoord.x * tw) / _texture2dDesc.Width;
		vertices[i].texcoord.y = (texPos.y + vertices[i].texcoord.y * th) / _texture2dDesc.Height;
	}

	D3D11_MAPPED_SUBRESOURCE msr;
	dc->Map(_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertices, sizeof(vertices));
	dc->Unmap(_vertexBuffer.Get(), 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(_inputLayout.Get());

	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);

	dc->PSSetShaderResources(0, 1, _srv.GetAddressOf());

	dc->Draw(4, 0);
}

void SpriteResource::Blit(ID3D11DeviceContext* immediateContext, 
	ID3D11ShaderResourceView** shaderResourceView, 
	uint32_t startSlot, uint32_t numViews,
	ID3D11PixelShader* pixelShader)
{
	immediateContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->IASetInputLayout(nullptr);

	immediateContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
	pixelShader != nullptr ?
		immediateContext->PSSetShader(pixelShader, nullptr, 0) :
		immediateContext->PSSetShader(this->_pixelShader.Get(), nullptr, 0);

	// シェーダーリソースビューの設定
	if (numViews != 0)
		immediateContext->PSSetShaderResources(startSlot, numViews, shaderResourceView);

	immediateContext->Draw(4, 0);

	// シェーダーリソースビューのクリア
	ID3D11ShaderResourceView* srvs[] = { nullptr };
	for (uint32_t i = 0; i < numViews; ++i)
	{
		immediateContext->PSSetShaderResources(startSlot + i, 1, srvs);
	}
	immediateContext->VSSetShader(nullptr, nullptr, 0);
}
