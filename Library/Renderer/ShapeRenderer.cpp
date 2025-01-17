#include "ShapeRenderer.h"

#include <memory>

#include "../../Library/ResourceManager/GpuResourceManager.h"
#include "../../Library/HRTrace.h"

class ShapeRendererData
{
public:
	ShapeRendererData(ID3D11Device* device)
	{
		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点シェーダー
		GpuResourceManager::CreateVsFromCso(
			device,
			"Data/Shader/ShapeRendererVS.cso",
			vertexShader.ReleaseAndGetAddressOf(),
			inputLayout.ReleaseAndGetAddressOf(),
			inputElementDesc,
			_countof(inputElementDesc)
		);

		// ピクセルシェーダー
		GpuResourceManager::CreatePsFromCso(
			device,
			"Data/Shader/ShapeRendererPS.cso",
			pixelShader.ReleaseAndGetAddressOf()
		);

		// 定数バッファ
		(void)GpuResourceManager::CreateConstantBuffer(
			device,
			sizeof(CbMesh),
			constantBuffer.GetAddressOf());

		// 箱メッシュ生成
		CreateBoxMesh(device, 1.0f, 1.0f, 1.0f);

		// 球メッシュ生成
		CreateSphereMesh(device, 1.0f, 32);

		// 円柱メッシュ生成
		CreateCylinderMesh(device, 1.0f, 1.0f, 16);
	}
	~ShapeRendererData() {}

	// 箱描画
	void DrawBox(
		const Vector3& position,
		const Vector3& angle,
		const Vector3& size,
		const Vector4& color)
	{
		Instance& instance = instances.emplace_back();
		instance.mesh = &boxMesh;
		instance.color = color;

		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(size.x, size.y, size.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMStoreFloat4x4(&instance.worldTransform, S * R * T);
	}
	void DrawBox(
		const Vector3& position,
		const Quaternion& oriental,
		const Vector3& size,
		const Vector4& color)
	{
		Instance& instance = instances.emplace_back();
		instance.mesh = &boxMesh;
		instance.color = color;

		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(size.x, size.y, size.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&oriental));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMStoreFloat4x4(&instance.worldTransform, S * R * T);
	}
	void DrawBox(
		const DirectX::XMFLOAT4X4& matrix,
		const Vector4& color)
	{
		Instance& instance = instances.emplace_back();
		instance.mesh = &boxMesh;
		instance.color = color;
		instance.worldTransform = matrix;
	}

	// 球描画
	void DrawSphere(
		const Vector3& position,
		float radius,
		const Vector4& color)
	{
		Instance& instance = instances.emplace_back();
		instance.mesh = &sphereMesh;
		instance.color = color;

		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(radius, radius, radius);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMStoreFloat4x4(&instance.worldTransform, S * T);
	}
	void DrawSphere(
		const DirectX::XMVECTOR& position,
		float radius,
		const Vector4& color)
	{
		Instance& instance = instances.emplace_back();
		instance.mesh = &sphereMesh;
		instance.color = color;

		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(radius, radius, radius);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(position);
		DirectX::XMStoreFloat4x4(&instance.worldTransform, S * T);
	}

	// カプセル描画
	void DrawCapsule(
		const DirectX::XMFLOAT4X4& transform,
		float radius,
		float height,
		const Vector4& color)
	{
		DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

		// 上半球
		{
			DirectX::XMVECTOR Position = DirectX::XMVector3Transform(DirectX::XMVectorSet(0, height * 0.5f, 0, 0), Transform);
			DrawSphere(Position, radius, color);
		}
		// 円柱
		{
			Instance& instance = instances.emplace_back();
			instance.mesh = &cylinderMesh;
			DirectX::XMMATRIX World;
			World.r[0] = DirectX::XMVectorScale(Transform.r[0], radius);
			World.r[1] = DirectX::XMVectorScale(Transform.r[1], height);
			World.r[2] = DirectX::XMVectorScale(Transform.r[2], radius);
			World.r[3] = Transform.r[3];
			DirectX::XMStoreFloat4x4(&instance.worldTransform, World);
			instance.color = color;
		}
		// 下半球
		{
			DirectX::XMVECTOR Position = DirectX::XMVector3Transform(DirectX::XMVectorSet(0, -height * 0.5f, 0, 0), Transform);
			DrawSphere(Position, radius, color);
		}
	}

	// 描画実行
	void Render(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection)
	{
		// シェーダー設定
		dc->VSSetShader(vertexShader.Get(), nullptr, 0);
		dc->PSSetShader(pixelShader.Get(), nullptr, 0);
		dc->IASetInputLayout(inputLayout.Get());

		static constexpr int CBIndex = 0;
		Microsoft::WRL::ComPtr<ID3D11Buffer> cacheBuffer;
		dc->PSGetConstantBuffers(CBIndex, 1, cacheBuffer.ReleaseAndGetAddressOf());
		// 定数バッファ設定
		dc->VSSetConstantBuffers(CBIndex, 1, constantBuffer.GetAddressOf());

		// ビュープロジェクション行列作成
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
		DirectX::XMMATRIX VP = V * P;

		// プリミティブ設定
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const Instance& instance : instances)
		{
			// 頂点バッファ設定
			uint32_t stride{ sizeof(Vertex) };
			uint32_t offset{ 0 };
			dc->IASetVertexBuffers(0, 1, instance.mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
			dc->IASetIndexBuffer(instance.mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			// ワールドビュープロジェクション行列作成
			DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&instance.worldTransform);
			DirectX::XMMATRIX WVP = W * VP;

			// 定数バッファ更新
			CbMesh cbMesh;
			DirectX::XMStoreFloat4x4(&cbMesh.worldViewProjection, WVP);
			cbMesh.color = instance.color;

			dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

			// 描画
			D3D11_BUFFER_DESC buffer_desc{};
			instance.mesh->indexBuffer->GetDesc(&buffer_desc);
			dc->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
		}
		instances.clear();

		dc->VSSetShader(NULL, 0, 0);
		dc->PSSetShader(NULL, 0, 0);
		dc->PSSetConstantBuffers(CBIndex, 1, cacheBuffer.GetAddressOf());
	}

private:
	struct Vertex
	{
		Vector3 position{};
		Vector3 normal{};
	};

	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer{};
		Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer{};
		UINT									vertexCount{};
	};

	struct Instance
	{
		Mesh* mesh = nullptr;
		DirectX::XMFLOAT4X4		worldTransform{};
		Vector4		color{};
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4		worldViewProjection{};
		Vector4		color{};
	};

	// メッシュ生成
	void CreateMesh(ID3D11Device* device,
		const std::vector<Vertex>& vertices,
		const std::vector<uint32_t>& indices,
		Mesh& mesh)
	{
		HRESULT hr{ S_OK };

		D3D11_BUFFER_DESC buffer_desc{};
		D3D11_SUBRESOURCE_DATA subresource_data{};
		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = vertices.data();
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.vertexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresource_data.pSysMem = indices.data();
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.indexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 箱メッシュ作成
	void CreateBoxMesh(ID3D11Device* device, float width, float height, float depth)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		vertices.resize(24);
		indices.resize(36);
		int face;

		// top-side
		// 0---------1
		// |         |
		// |   -Y    |
		// |         |
		// 2---------3
		face = 0;
		vertices[face * 4 + 0].position = Vector3(-0.5f, +0.5f, +0.5f);
		vertices[face * 4 + 1].position = Vector3(+0.5f, +0.5f, +0.5f);
		vertices[face * 4 + 2].position = Vector3(-0.5f, +0.5f, -0.5f);
		vertices[face * 4 + 3].position = Vector3(+0.5f, +0.5f, -0.5f);
		vertices[face * 4 + 0].normal = Vector3(+0.0f, +1.0f, +0.0f);
		vertices[face * 4 + 1].normal = Vector3(+0.0f, +1.0f, +0.0f);
		vertices[face * 4 + 2].normal = Vector3(+0.0f, +1.0f, +0.0f);
		vertices[face * 4 + 3].normal = Vector3(+0.0f, +1.0f, +0.0f);
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 1;
		indices[face * 6 + 2] = face * 4 + 2;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 3;
		indices[face * 6 + 5] = face * 4 + 2;

		// bottom-side
		// 0---------1
		// |         |
		// |   -Y    |
		// |         |
		// 2---------3
		face += 1;
		vertices[face * 4 + 0].position = Vector3(-0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 1].position = Vector3(+0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 2].position = Vector3(-0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 3].position = Vector3(+0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 0].normal = Vector3(+0.0f, -1.0f, +0.0f);
		vertices[face * 4 + 1].normal = Vector3(+0.0f, -1.0f, +0.0f);
		vertices[face * 4 + 2].normal = Vector3(+0.0f, -1.0f, +0.0f);
		vertices[face * 4 + 3].normal = Vector3(+0.0f, -1.0f, +0.0f);
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 2;
		indices[face * 6 + 2] = face * 4 + 1;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 2;
		indices[face * 6 + 5] = face * 4 + 3;

		// front-side
		// 0---------1
		// |         |
		// |   +Z    |
		// |         |
		// 2---------3
		face += 1;
		vertices[face * 4 + 0].position = Vector3(-0.5f, +0.5f, -0.5f);
		vertices[face * 4 + 1].position = Vector3(+0.5f, +0.5f, -0.5f);
		vertices[face * 4 + 2].position = Vector3(-0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 3].position = Vector3(+0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 0].normal = Vector3(+0.0f, +0.0f, -1.0f);
		vertices[face * 4 + 1].normal = Vector3(+0.0f, +0.0f, -1.0f);
		vertices[face * 4 + 2].normal = Vector3(+0.0f, +0.0f, -1.0f);
		vertices[face * 4 + 3].normal = Vector3(+0.0f, +0.0f, -1.0f);
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 1;
		indices[face * 6 + 2] = face * 4 + 2;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 3;
		indices[face * 6 + 5] = face * 4 + 2;

		// back-side
		// 0---------1
		// |         |
		// |   +Z    |
		// |         |
		// 2---------3
		face += 1;
		vertices[face * 4 + 0].position = Vector3(-0.5f, +0.5f, +0.5f);
		vertices[face * 4 + 1].position = Vector3(+0.5f, +0.5f, +0.5f);
		vertices[face * 4 + 2].position = Vector3(-0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 3].position = Vector3(+0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 0].normal = Vector3(+0.0f, +0.0f, +1.0f);
		vertices[face * 4 + 1].normal = Vector3(+0.0f, +0.0f, +1.0f);
		vertices[face * 4 + 2].normal = Vector3(+0.0f, +0.0f, +1.0f);
		vertices[face * 4 + 3].normal = Vector3(+0.0f, +0.0f, +1.0f);
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 2;
		indices[face * 6 + 2] = face * 4 + 1;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 2;
		indices[face * 6 + 5] = face * 4 + 3;

		// right-side
		// 0---------1
		// |         |      
		// |   -X    |
		// |         |
		// 2---------3
		face += 1;
		vertices[face * 4 + 0].position = Vector3(+0.5f, +0.5f, -0.5f);
		vertices[face * 4 + 1].position = Vector3(+0.5f, +0.5f, +0.5f);
		vertices[face * 4 + 2].position = Vector3(+0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 3].position = Vector3(+0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 0].normal = Vector3(+1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 1].normal = Vector3(+1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 2].normal = Vector3(+1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 3].normal = Vector3(+1.0f, +0.0f, +0.0f);
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 1;
		indices[face * 6 + 2] = face * 4 + 2;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 3;
		indices[face * 6 + 5] = face * 4 + 2;

		// left-side
		// 0---------1
		// |         |      
		// |   -X    |
		// |         |
		// 2---------3
		face += 1;
		vertices[face * 4 + 0].position = Vector3(-0.5f, +0.5f, -0.5f);
		vertices[face * 4 + 1].position = Vector3(-0.5f, +0.5f, +0.5f);
		vertices[face * 4 + 2].position = Vector3(-0.5f, -0.5f, -0.5f);
		vertices[face * 4 + 3].position = Vector3(-0.5f, -0.5f, +0.5f);
		vertices[face * 4 + 0].normal = Vector3(-1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 1].normal = Vector3(-1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 2].normal = Vector3(-1.0f, +0.0f, +0.0f);
		vertices[face * 4 + 3].normal = Vector3(-1.0f, +0.0f, +0.0f);
		indices[face * 6 + 0] = face * 4 + 0;
		indices[face * 6 + 1] = face * 4 + 2;
		indices[face * 6 + 2] = face * 4 + 1;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 2;
		indices[face * 6 + 5] = face * 4 + 3;

		// メッシュ生成
		CreateMesh(device, vertices, indices, boxMesh);
	}

	// 球メッシュ作成
	void CreateSphereMesh(ID3D11Device* device, float radius, int subdivisions)
	{
		const u_int polygonalVertical = 10;
		const u_int polygonalHorizontal = 10;

		std::vector<Vertex> vertices;
		std::vector<u_int> indices;

		float verticalDiv = 2.0f * DirectX::XM_PI / static_cast<float>(polygonalVertical);
		float horizontallDiv = 2.0f * DirectX::XM_PI / static_cast<float>(polygonalHorizontal);
		float roll = 0.0f;
		float dis = 1.0f;
		float posY = 0.0f;

		Vertex vertex{};
		u_int base_index = 0;

		// 球体上部
		for (u_int n = 0; n < polygonalVertical / 2.0f; ++n)
		{
			roll = verticalDiv / 2.0f * n;
			dis = cosf(roll);
			posY = sinf(roll);
			for (u_int i = 0; i < polygonalHorizontal; ++i)
			{
				float x = dis * cosf(i * horizontallDiv);
				float z = dis * sinf(i * horizontallDiv);
				vertex.position = Vector3(x, +posY, z);
				vertex.normal = Vector3(x, posY, z);
				vertices.push_back(vertex);
			}
		}
		// 上部の先端
		vertex.position = Vector3(0, 1, 0);
		vertex.normal = Vector3(0, 1, 0);
		vertices.push_back(vertex);

		// 球体上部の先端以外
		for (u_int n = 0; n < polygonalVertical / 2.0f - 1; ++n)
		{
			for (u_int i = 0; i < polygonalHorizontal - 1; ++i)
			{
				indices.push_back(n * polygonalHorizontal + i);
				indices.push_back(n * polygonalHorizontal + polygonalHorizontal + i);
				indices.push_back(n * polygonalHorizontal + i + 1);
				indices.push_back(n * polygonalHorizontal + polygonalHorizontal + i);
				indices.push_back(n * polygonalHorizontal + polygonalHorizontal + i + 1);
				indices.push_back(n * polygonalHorizontal + i + 1);
			}
			// 行の分かれ目
			indices.push_back(n * polygonalHorizontal + polygonalHorizontal - 1);
			indices.push_back(n * polygonalHorizontal + polygonalHorizontal * 2 - 1);
			indices.push_back(n * polygonalHorizontal + polygonalHorizontal);
			indices.push_back(n * polygonalHorizontal + polygonalHorizontal - 1);
			indices.push_back(n * polygonalHorizontal + polygonalHorizontal);
			indices.push_back(n * polygonalHorizontal);
			base_index = n * polygonalHorizontal + polygonalHorizontal;
		}
		// 球体上部の先端
		for (u_int i = 0; i < polygonalHorizontal; ++i)
		{
			indices.push_back(base_index + polygonalHorizontal - i - 1);
			indices.push_back(base_index + polygonalHorizontal);
			indices.push_back(base_index + polygonalHorizontal - i);
		}
		indices.push_back(base_index - 1 + polygonalHorizontal);
		indices.push_back(base_index + polygonalHorizontal);
		indices.push_back(base_index);

		// vertexの最後の番号取得
		base_index = static_cast<u_int>(vertices.size());

		// 球体下部
		// すでに中心は配置済み
		for (u_int n = 1; n < polygonalVertical / 2.0f; ++n)
		{
			roll = verticalDiv / 2.0f * n;
			dis = cosf(roll);
			posY = sinf(roll);
			for (u_int i = 0; i < polygonalHorizontal; ++i)
			{
				float x = dis * cosf(i * horizontallDiv);
				float z = dis * sinf(i * horizontallDiv);
				vertex.position = Vector3(x, -posY, z);
				vertex.normal = Vector3(x, -posY, z);
				vertices.push_back(vertex);
			}
		}
		// 下部の先端
		vertex.position = Vector3(0, -1, 0);
		vertex.normal = Vector3(0, -1, 0);
		vertices.push_back(vertex);

		// 球体下部の先端以外
		// 中心から一行目		
		for (u_int i = 0; i < polygonalVertical - 1; ++i)
		{
			indices.push_back(i + 1);
			indices.push_back(base_index + i);
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(base_index + i + 1);
			indices.push_back(base_index + i);
		}
		// 行の分かれ目
		indices.push_back(0);
		indices.push_back(base_index);
		indices.push_back(base_index + polygonalVertical - 1);
		indices.push_back(base_index + polygonalVertical - 1);
		indices.push_back(polygonalVertical - 1);
		indices.push_back(0);
		// 中心から一行目移行			
		indices.push_back(base_index);
		indices.push_back(base_index + 1);
		indices.push_back(base_index + polygonalVertical);
		for (u_int n = 0; n < polygonalVertical / 2.0f - 2; ++n)
		{
			for (u_int i = 0; i < polygonalVertical - 1; ++i)
			{
				indices.push_back(n * polygonalHorizontal + base_index + i);
				indices.push_back(n * polygonalHorizontal + base_index + i + 1);
				indices.push_back(n * polygonalHorizontal + base_index + i + polygonalVertical);
				indices.push_back(n * polygonalHorizontal + base_index + i + 1);
				indices.push_back(n * polygonalHorizontal + base_index + i + polygonalVertical + 1);
				indices.push_back(n * polygonalHorizontal + base_index + i + polygonalVertical);
			}
			// 行の分かれ目
			indices.push_back(n * polygonalHorizontal + base_index + polygonalVertical - 1);
			indices.push_back(n * polygonalHorizontal + base_index);
			indices.push_back(n * polygonalHorizontal + base_index + polygonalVertical * 2 - 1);
			indices.push_back(n * polygonalHorizontal + base_index + polygonalVertical);
			indices.push_back(n * polygonalHorizontal + base_index + polygonalVertical * 2 - 1);
			indices.push_back(n * polygonalHorizontal + base_index);
		}

		// vertexの最後の番号取得
		base_index = static_cast<u_int>(vertices.size());

		// 球体下部の先端
		for (u_int i = 0; i < polygonalHorizontal - 1; ++i)
		{
			indices.push_back(base_index - 1);
			indices.push_back(base_index - 3 - i);
			indices.push_back(base_index - 2 - i);
		}
		indices.push_back(base_index - 1);
		indices.push_back(base_index - 2);
		indices.push_back(base_index - polygonalHorizontal - 1);

		// メッシュ生成
		CreateMesh(device, vertices, indices, sphereMesh);
	}

	// 円柱
	// 円柱の中心が{0,0,0}
	// height = 下部から上部までの長さ
	void CreateCylinderMesh(ID3D11Device* device, float radius, float height, int subdivisions)
	{
		std::vector<Vertex> vertices;
		std::vector<u_int> indices;

		float theta_step = DirectX::XM_2PI / subdivisions;

		// 頂点生成
		// 上部
		// 中心
		Vertex& upCenter = vertices.emplace_back();
		upCenter.position = { 0,height / 2.0f,0 };
		upCenter.normal = { 0,1,0 };
		for (int i = 0; i < subdivisions; ++i)
		{
			float theta = theta_step * i;
			Vertex& v = vertices.emplace_back();
			v.position.x = sinf(theta) * radius;
			v.position.y = height / 2.0f;
			v.position.z = cosf(theta) * radius;
			v.normal = { 0,1,0 };
		}
		// 下部
		// 中心
		Vertex& downCenter = vertices.emplace_back();
		downCenter.position = { 0,-height / 2.0f,0 };
		downCenter.normal = { 0,-1,0 };
		for (int i = 0; i < subdivisions; ++i)
		{
			float theta = theta_step * i;
			Vertex& v = vertices.emplace_back();
			v.position.x = sinf(theta) * radius;
			v.position.y = -height / 2.0f;
			v.position.z = cosf(theta) * radius;
			v.normal = { 0,-1,0 };
		}
		// 横
		for (int i = 0; i < subdivisions; ++i)
		{
			float theta = theta_step * i;
			// 下
			Vertex& down = vertices.emplace_back();
			down.normal = { sinf(theta),0.0f,cosf(theta) };
			down.position.x = down.normal.x * radius;
			down.position.y = -height / 2.0f;
			down.position.z = down.normal.z * radius;
			// 上
			Vertex& up = vertices.emplace_back();
			up.normal = { sinf(theta),0.0f,cosf(theta) };
			up.position.x = up.normal.x * radius;
			up.position.y = height / 2.0f;
			up.position.z = up.normal.z * radius;
		}

		// 頂点番号設定
		for (int i = 1; i < subdivisions; i++)
		{
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(i + 1);
		}
		indices.push_back(0);
		indices.push_back(subdivisions);
		indices.push_back(1);
		for (int i = 1; i < subdivisions + 1; i++)
		{
			indices.push_back(subdivisions + 1);
			indices.push_back(subdivisions + i + 1);
			indices.push_back(subdivisions + i);
		}
		indices.push_back(subdivisions + 1);
		indices.push_back(subdivisions * 2 + 1);
		indices.push_back(subdivisions + 2);
		int offset = subdivisions * 2 + 1 + 1;
		for (int i = 0; i < subdivisions * 2; i++)
		{
			indices.push_back(offset + i);
			indices.push_back(offset + i + 1);
			indices.push_back(offset + i + 2);
		}



		// メッシュ生成
		CreateMesh(device, vertices, indices, cylinderMesh);
	}

private:
	Mesh										boxMesh;
	Mesh										sphereMesh;
	Mesh										halfSphereMesh;
	Mesh										cylinderMesh;
	Mesh										boneMesh;
	std::vector<Instance>						instances;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer;
};


namespace ShapeRenderer
{
	std::unique_ptr<ShapeRendererData> shapeRendererData;
	void Initialize(ID3D11Device* device)
	{
		shapeRendererData = std::make_unique<ShapeRendererData>(device);
	}
	void DrawBox(const Vector3& position, const Vector3& angle, const Vector3& size, const Vector4& color)
	{
		shapeRendererData->DrawBox(position, angle, size, color);
	}
	void DrawBox(const Vector3& position, const Quaternion& oriental, const Vector3& size, const Vector4& color)
	{
		shapeRendererData->DrawBox(position, oriental, size, color);
	}
	void DrawBox(const DirectX::XMFLOAT4X4& matrix, const Vector4& color)
	{
		shapeRendererData->DrawBox(matrix, color);
	}
	void DrawSphere(const Vector3& position, float radius, const Vector4& color)
	{
		shapeRendererData->DrawSphere(position, radius, color);
	}
	void DrawSphere(const DirectX::XMVECTOR& position, float radius, const Vector4& color)
	{
		shapeRendererData->DrawSphere(position, radius, color);
	}
	void DrawCapsule(const DirectX::XMFLOAT4X4& transform, float radius, float height, const Vector4& color)
	{
		shapeRendererData->DrawCapsule(transform, radius, height, color);
	}
	void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
	{
		shapeRendererData->Render(dc, view, projection);
	}
}
