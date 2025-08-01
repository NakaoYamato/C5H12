#include "DecalRenderer.h"

#include "../../Library/HRTrace.h"
#include "../../Library/Graphics/GpuResourceManager.h"

// 初期設定
void DecalRenderer::Initialize(ID3D11Device* device, UINT width, UINT height)
{
	HRESULT hr{ S_OK };

	// デプスステンシルビューの生成
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = width;
		texture2d_desc.Height = height;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = texture2d_desc.Format;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, _depthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 制限用深度ステンシルステート生成
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	// 深度比較は行うが書き込まない
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// ステンシルの設定
	// 裏面描画時はステンシルを書き込む
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	// 表面描画時はステンシルを比較
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	hr = device->CreateDepthStencilState(&depthStencilDesc,
		_depthStencilState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// デカールの頂点バッファとインデックスバッファを作成
	CreateCubeCOMObject(device);

	// デカール定数バッファの作成
	GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(DecalConstants),
		_decalConstantBuffer.ReleaseAndGetAddressOf());
	// ジオメトリ定数バッファの作成
	GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(GeometryConstants),
		_geometryConstantBuffer.ReleaseAndGetAddressOf());

	// ジオメトリ頂点シェーダーの読み込み
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	GpuResourceManager::CreateVsFromCso(
		device,
		"./Data/Shader/DecalGeometryVS.cso",
		_geometryVertexShader.ReleaseAndGetAddressOf(),
		_geometryInputLayout.ReleaseAndGetAddressOf(),
		inputElementDesc, static_cast<UINT>(_countof(inputElementDesc)));

	// ピクセルシェーダーの読み込み
	GpuResourceManager::CreatePsFromCso(
		device,
		"./Data/Shader/DecalGeometryPS.cso",
		_geometrypixelShader.ReleaseAndGetAddressOf());
	GpuResourceManager::CreatePsFromCso(
		device,
		"./Data/Shader/DecalSpritePS.cso",
		_spritePixelShader.ReleaseAndGetAddressOf());
}
// 描画申請
void DecalRenderer::Draw(Decal* decal, const DirectX::XMFLOAT4X4& world, const Vector4& color)
{
	// 描画情報を追加
	_drawInfos.push_back({ decal, &world, &color });
}
// 描画処理
void DecalRenderer::Render(GBuffer* gbuffer, ID3D11Device* device, const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> copyDepthStencilSRV;
	{
		HRESULT hr;
		Microsoft::WRL::ComPtr<ID3D11Resource> pSourceResource;
		gbuffer->GetDepthSRV()->GetResource(pSourceResource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pSourceTexture;
		hr = pSourceResource.As(&pSourceTexture);
		if (FAILED(hr)) {
			// エラー処理
			return;
		}

		D3D11_TEXTURE2D_DESC desc;
		pSourceTexture->GetDesc(&desc);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDestTexture;
		hr = device->CreateTexture2D(&desc, nullptr, pDestTexture.GetAddressOf());
		if (FAILED(hr)) {
			// エラー処理
			return;
		}

		dc->CopyResource(pDestTexture.Get(), pSourceTexture.Get());
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(pDestTexture.Get(),
			&srvDesc,
			copyDepthStencilSRV.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			// エラー処理
			return;
		}
	}

	// 出力先を設定
	{
		std::vector<ID3D11RenderTargetView*> tempRTVs;
		tempRTVs.push_back(gbuffer->GetRenderTargetView(GBUFFER_COLOR_MAP_INDEX).Get());
		tempRTVs.push_back(gbuffer->GetRenderTargetView(GBUFFER_NORMAL_MAP_INDEX).Get());
		dc->OMSetRenderTargets(2, tempRTVs.data(), gbuffer->GetDepthStencilView().Get());
		// ステンシルをクリア
		dc->ClearDepthStencilView(gbuffer->GetDepthStencilView().Get(), D3D11_CLEAR_STENCIL, 0.0f, 0);
	}

	// ビューポート設定
	{
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(gbuffer->GetWidth());
		viewport.Height = static_cast<float>(gbuffer->GetHeight());
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		dc->RSSetViewports(1, &viewport);
	}

	// サンプラーを設定
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::MultipleRenderTargets), nullptr, 0xFFFFFFFF);
	
	// デカール描画
	for (const DrawInfo& drawInfo : _drawInfos)
	{
		if (!drawInfo.decal) continue;
		if (!drawInfo.world) continue;
		if (!drawInfo.color) continue;

		DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(drawInfo.world);

		// シェーダー内でGBufferのテクスチャのワールド座標を取得するために深度値情報を送信
		dc->PSSetShaderResources(GBUFFER_DEPTH_SRV_INDEX, 1, copyDepthStencilSRV.GetAddressOf());
		// 情報の設定
		{
			dc->PSSetShaderResources(DECAL_COLOR_SRV_INDEX, 1, drawInfo.decal->GetColorSRV().GetAddressOf());
			dc->PSSetShaderResources(DECAL_NORMAL_SRV_INDEX, 1, drawInfo.decal->GetNormalSRV().GetAddressOf());

			DecalConstants decalConstant{};
			{
				// ボックスの向きを保存
				decalConstant.direction = Vector3(Vector3::Up).TransformNormal(World).Normalize();
				decalConstant.direction.w = 0;
				// ボックスの空間に変換するための行列
				DirectX::XMMATRIX V = DirectX::XMMatrixInverse(nullptr, World);
				DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(1, 1, 0, 1);
				DirectX::XMStoreFloat4x4(&decalConstant.inverseTransform, V * P);
			}

			dc->UpdateSubresource(_decalConstantBuffer.Get(), 0, 0, &decalConstant, 0, 0);
			dc->PSSetConstantBuffers(DECAL_CONSTANT_INDEX, 1, _decalConstantBuffer.GetAddressOf());
		}

		{
			//	頂点シェーダー等の設定
			dc->VSSetShader(_geometryVertexShader.Get(), nullptr, 0);
			dc->IASetInputLayout(_geometryInputLayout.Get());
			dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//	裏面描画してステンシル値1を書き込む
			{
				dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullFront));
				dc->OMSetDepthStencilState(_depthStencilState.Get(), 1);
				dc->PSSetShader(nullptr, nullptr, 0);
				DrawGeometry(dc, *drawInfo.world, { 1, 1, 1, 1 });
			}

			// ボックスの内部判定を行う
			//OBB obb;
			//obb.position.x = decal.translation.x;
			//obb.position.y = decal.translation.y;
			//obb.position.z = decal.translation.z;
			//obb.position.w = 1;
			//obb.length.x = decal.scaling.x;
			//obb.length.y = decal.scaling.y;
			//obb.length.z = decal.scaling.z;
			//obb.length.w = 0;
			//DirectX::XMStoreFloat4(&obb.axis[0], DirectX::XMVector3Normalize(World.r[0]));
			//DirectX::XMStoreFloat4(&obb.axis[1], DirectX::XMVector3Normalize(World.r[1]));
			//DirectX::XMStoreFloat4(&obb.axis[2], DirectX::XMVector3Normalize(World.r[2]));
			//float length = length_point_to_obb(obb, get_camera_position());
			//if (length < FLT_EPSILON)
			//{
			//	// フルスクリーン描画
			//	dc->OMSetDepthStencilState(decal_depth_stencil_state.Get(), 0);
			//	dc->RSSetState(get_renderdata()->rasterizer_states[static_cast<size_t>(render_data::RASTER_STATE::CULL_NONE)].Get());

			//	dc->VSSetShader(sprite_vertex_shader.Get(), nullptr, 0);
			//	dc->IASetInputLayout(sprite_input_layout.Get());
			//	dc->PSSetShader(gbuffer_decal_sprite_pixel_shader.Get(), nullptr, 0);
			//	gbuffer_decal_sprite->render(dc.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			//}
			//else
			{
				//	表面描画してステンシル値1と比較
				dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));
				dc->OMSetDepthStencilState(_depthStencilState.Get(), 0);
				dc->PSSetShader(_geometrypixelShader.Get(), nullptr, 0);
				DrawGeometry(dc, *drawInfo.world, *drawInfo.color);
			}
		}
	}
	// 情報をクリア
	_drawInfos.clear();
}
// 描画用COMオブジェクトの生成
void DecalRenderer::CreateCubeCOMObject(ID3D11Device* device)
{
	const UINT vertexCount = 24;
	const UINT indexCount = 36;

	GeometryVertex vertices[vertexCount]{};
	uint32_t indices[indexCount]{};

	uint32_t face{ 0 };

	// top-side
	face = 0;
	vertices[face * 4 + 0].position = { +0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 1].position = { -0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 2].position = { +0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 3].position = { -0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 0].normal = { +0.0f, +1.0f, +0.0f };
	vertices[face * 4 + 1].normal = { +0.0f, +1.0f, +0.0f };
	vertices[face * 4 + 2].normal = { +0.0f, +1.0f, +0.0f };
	vertices[face * 4 + 3].normal = { +0.0f, +1.0f, +0.0f };
	vertices[face * 4 + 0].texcoord = { +0.0f, +0.0f };
	vertices[face * 4 + 1].texcoord = { +1.0f, +0.0f };
	vertices[face * 4 + 2].texcoord = { +0.0f, +1.0f };
	vertices[face * 4 + 3].texcoord = { +1.0f, +1.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// bottom-side
	face += 1;
	vertices[face * 4 + 0].position = { -0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 1].position = { +0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 0].normal = { +0.0f, -1.0f, +0.0f };
	vertices[face * 4 + 1].normal = { +0.0f, -1.0f, +0.0f };
	vertices[face * 4 + 2].normal = { +0.0f, -1.0f, +0.0f };
	vertices[face * 4 + 3].normal = { +0.0f, -1.0f, +0.0f };
	vertices[face * 4 + 0].texcoord = { +0.0f, +0.0f };
	vertices[face * 4 + 1].texcoord = { +1.0f, +0.0f };
	vertices[face * 4 + 2].texcoord = { +0.0f, +1.0f };
	vertices[face * 4 + 3].texcoord = { +1.0f, +1.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// front-side
	face += 1;
	vertices[face * 4 + 0].position = { +0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 1].position = { -0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 2].position = { +0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 3].position = { -0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 0].normal = { +0.0f, +0.0f, -1.0f };
	vertices[face * 4 + 1].normal = { +0.0f, +0.0f, -1.0f };
	vertices[face * 4 + 2].normal = { +0.0f, +0.0f, -1.0f };
	vertices[face * 4 + 3].normal = { +0.0f, +0.0f, -1.0f };
	vertices[face * 4 + 0].texcoord = { +0.0f, +0.0f };
	vertices[face * 4 + 1].texcoord = { +1.0f, +0.0f };
	vertices[face * 4 + 2].texcoord = { +0.0f, +1.0f };
	vertices[face * 4 + 3].texcoord = { +1.0f, +1.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// back-side
	face += 1;
	vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 2].position = { -0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 0].normal = { +0.0f, +0.0f, +1.0f };
	vertices[face * 4 + 1].normal = { +0.0f, +0.0f, +1.0f };
	vertices[face * 4 + 2].normal = { +0.0f, +0.0f, +1.0f };
	vertices[face * 4 + 3].normal = { +0.0f, +0.0f, +1.0f };
	vertices[face * 4 + 0].texcoord = { +0.0f, +0.0f };
	vertices[face * 4 + 1].texcoord = { +1.0f, +0.0f };
	vertices[face * 4 + 2].texcoord = { +0.0f, +1.0f };
	vertices[face * 4 + 3].texcoord = { +1.0f, +1.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// right-side
	face += 1;
	vertices[face * 4 + 1].position = { +0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 0].position = { +0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 2].position = { +0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 0].normal = { +1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 1].normal = { +1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 2].normal = { +1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 3].normal = { +1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 0].texcoord = { +0.0f, +0.0f };
	vertices[face * 4 + 1].texcoord = { +1.0f, +0.0f };
	vertices[face * 4 + 2].texcoord = { +0.0f, +1.0f };
	vertices[face * 4 + 3].texcoord = { +1.0f, +1.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// left-side
	face += 1;
	vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 1].position = { -0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 3].position = { -0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 0].normal = { -1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 1].normal = { -1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 2].normal = { -1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 3].normal = { -1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 0].texcoord = { +0.0f, +0.0f };
	vertices[face * 4 + 1].texcoord = { +1.0f, +0.0f };
	vertices[face * 4 + 2].texcoord = { +0.0f, +1.0f };
	vertices[face * 4 + 3].texcoord = { +1.0f, +1.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	D3D11_SUBRESOURCE_DATA subresource_data{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(GeometryVertex) * vertexCount);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, _geometryVertexBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indexCount);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresource_data.pSysMem = indices;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, _geometryIndexBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}
// ジオメトリの描画
void DecalRenderer::DrawGeometry(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& world, const Vector4& color)
{
	uint32_t stride{ sizeof(GeometryVertex) };
	uint32_t offset{ 0 };
	dc->IASetVertexBuffers(0, 1, _geometryVertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(_geometryIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	GeometryConstants data{ world, color };
	dc->UpdateSubresource(_geometryConstantBuffer.Get(), 0, 0, &data, 0, 0);
	dc->VSSetConstantBuffers(GEOMETRY_CONSTANT_INDEX, 1, _geometryConstantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(GEOMETRY_CONSTANT_INDEX, 1, _geometryConstantBuffer.GetAddressOf());

	D3D11_BUFFER_DESC bufferDesc{};
	_geometryIndexBuffer->GetDesc(&bufferDesc);
	dc->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
}
