#include "DebugRenderer.h"

#include "../../Library/HRTrace.h"
#include "../../Library/ResourceManager/GpuResourceManager.h"
#include "../../Library/Math/Quaternion.h"

#include "../DebugSupporter.h"

// コンストラクタ
DebugRenderer::DebugRenderer(ID3D11Device* device)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点シェーダー
	GpuResourceManager::CreateVsFromCso(
		device,
		"./Data/Shader/DebugRendererVS.cso",
		_vertexShader.ReleaseAndGetAddressOf(),
		_inputLayout.ReleaseAndGetAddressOf(),
		inputElementDesc,
		_countof(inputElementDesc)
	);

	// ピクセルシェーダー
	GpuResourceManager::CreatePsFromCso(
		device,
		"./Data/Shader/DebugRendererPS.cso",
		_pixelShader.ReleaseAndGetAddressOf()
	);

	// 定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(CbMesh),
		_constantBuffer.GetAddressOf());

	// 箱メッシュ生成
	CreateBoxMesh(device, 1.0f, 1.0f, 1.0f);

	// 球メッシュ生成
	CreateSphereMesh(device, 1.0f, 32);

	// 半球メッシュ生成
	CreateHalfSphereMesh(device, 1.0f, 32);

	// 円柱メッシュ生成
	CreateCylinderMesh(device, 1.0f, 1.0f, 0.0f, 1.0f, 32);

	// 骨メッシュ生成
	CreateBoneMesh(device, 1.0f);

	// 矢印メッシュ作成
	CreateArrowMesh(device);

	// 軸メッシュ作成
	CreateAxis(device);

	// グリッド描画用頂点バッファ作成
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(Vector3) * VertexCapacity;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = device->CreateBuffer(&desc, nullptr, _gridVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

// 箱描画
void DebugRenderer::DrawBox(
	const Vector3& position,
	const Vector3& angle,
	const Vector3& size,
	const Vector4& color)
{
	Instance& instance = _instances.emplace_back();
	instance.mesh = &_boxMesh;
	instance.color = color;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(size.x, size.y, size.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&instance.worldTransform, S * R * T);
}

// 箱描画
void DebugRenderer::DrawBox(const DirectX::XMFLOAT4X4& transform, const Vector4& color)
{
	Instance& instance = _instances.emplace_back();
	instance.mesh = &_boxMesh;
	instance.color = color;
	instance.worldTransform = transform;
}

// 球描画
void DebugRenderer::DrawSphere(
	const Vector3& position,
	float radius,
	const Vector4& color)
{
	Instance& instance = _instances.emplace_back();
	instance.mesh = &_sphereMesh;
	instance.color = color;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(radius, radius, radius);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&instance.worldTransform, S * T);
}

// カプセル描画
void DebugRenderer::DrawCapsule(
	const DirectX::XMFLOAT4X4& transform,
	float radius,
	float height,
	const Vector4& color)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	// 上半球
	{
		Instance& instance = _instances.emplace_back();
		instance.mesh = &_halfSphereMesh;
		DirectX::XMVECTOR Position = DirectX::XMVector3Transform(DirectX::XMVectorSet(0, height * 0.5f, 0, 0), Transform);
		DirectX::XMMATRIX World = DirectX::XMMatrixScaling(radius, radius, radius);
		World.r[3] = DirectX::XMVectorSetW(Position, 1.0f);
		DirectX::XMStoreFloat4x4(&instance.worldTransform, World);
		instance.color = color;
	}
	// 円柱
	{
		Instance& instance = _instances.emplace_back();
		instance.mesh = &_cylinderMesh;
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
		Instance& instance = _instances.emplace_back();
		instance.mesh = &_halfSphereMesh;
		DirectX::XMMATRIX World = DirectX::XMMatrixRotationX(DirectX::XM_PI);
		DirectX::XMVECTOR Position = DirectX::XMVector3Transform(DirectX::XMVectorSet(0, -height * 0.5f, 0, 0), Transform);
		Transform.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
		World = DirectX::XMMatrixMultiply(World, Transform);
		World.r[0] = DirectX::XMVectorScale(World.r[0], radius);
		World.r[1] = DirectX::XMVectorScale(World.r[1], radius);
		World.r[2] = DirectX::XMVectorScale(World.r[2], radius);
		World.r[3] = DirectX::XMVectorSetW(Position, 1.0f);
		DirectX::XMStoreFloat4x4(&instance.worldTransform, World);
		instance.color = color;
	}
}

// カプセル描画
void DebugRenderer::DrawCapsule(const Vector3& start, const Vector3& end, float radius, const Vector4& color)
{
	// 始点、終点
	{
		DebugRenderer::DrawSphere(start, radius, color);
		DebugRenderer::DrawSphere(end, radius, color);
	}

	// 円柱
	{
		// 始点の位置行列作成
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&start));
		// 回転行列作成
		Vector3 target = end - start;
		Quaternion q = Quaternion::LookAt(start, Quaternion::AxisY, end);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&q));
		// 高さ
		float length = Vector3::Length(target);

		Instance& instance = _instances.emplace_back();
		instance.mesh = &_cylinderMesh;
		DirectX::XMMATRIX World = R * T;
		World.r[0] = DirectX::XMVectorScale(World.r[0], radius);
		World.r[1] = DirectX::XMVectorScale(World.r[1], length);
		World.r[2] = DirectX::XMVectorScale(World.r[2], radius);
		DirectX::XMStoreFloat4x4(&instance.worldTransform, World);
		instance.color = color;
	}
}

// 骨描画
void DebugRenderer::DrawBone(
	const DirectX::XMFLOAT4X4& transform,
	float length,
	const Vector4& color)
{
	Instance& instance = _instances.emplace_back();
	instance.mesh = &_boneMesh;
	instance.color = color;

	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
	W.r[0] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(W.r[0]), length);
	W.r[1] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(W.r[1]), length);
	W.r[2] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(W.r[2]), length);
	DirectX::XMStoreFloat4x4(&instance.worldTransform, W);
}

void DebugRenderer::DrawArrow(
	const Vector3& start,
	const Vector3& target,
	float radius,
	const Vector4& color)
{
	// startからtargetまでの姿勢
	Quaternion q = Quaternion::LookAt(start, Quaternion::AxisX, target);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&q));

	{
		Instance& instance = _instances.emplace_back();
		instance.mesh = &_arrowMesh;

		Vector3 vec = (target - start);
		// Y軸をstartからtargetにする
		DirectX::XMVECTOR Up = DirectX::XMVectorSet(vec.x, vec.y, vec.z, 0.0f);
		DirectX::XMVECTOR Front = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		// X軸生成
		DirectX::XMVECTOR Right = DirectX::XMVector3Cross(Up, Front);
		// 外積の計算が失敗しているならばUp == Front
		if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Right)) == 0.0f)
		{
			Front = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			Right = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		}
		else
		{
			Right = DirectX::XMVector3Normalize(Right);
		}

		// Y軸をstartからtargetにする
		DirectX::XMMATRIX Transform{};
		Transform.r[0] = Right;
		Transform.r[1] = Up;
		Transform.r[2] = Front;
		Transform.r[3] = DirectX::XMVectorSet(start.x, start.y, start.z, 1.0f);

		DirectX::XMStoreFloat4x4(&instance.worldTransform, Transform);
		instance.color = color;
	}
}

// 軸描画
void DebugRenderer::DrawAxis(const DirectX::XMFLOAT4X4& transform)
{
	// X軸
	{
		Instance& instance = _instances.emplace_back();
		instance.mesh = &_axisMesh[0];
		instance.color = { 1,0,0,1 };
		instance.worldTransform = transform;
	}
	// Y軸
	{
		Instance& instance = _instances.emplace_back();
		instance.mesh = &_axisMesh[1];
		instance.color = { 0,1,0,1 };
		instance.worldTransform = transform;
	}
	// Z軸
	{
		Instance& instance = _instances.emplace_back();
		instance.mesh = &_axisMesh[2];
		instance.color = { 0,0,1,1 };
		instance.worldTransform = transform;
	}
}

// グリッド描画
void DebugRenderer::DrawGrid(int subdivisions, float scale)
{
	auto AddVertex = [&](const Vector3& position)
		{
			Vector3& v = _gridVertices.emplace_back();
			v = position;
		};

	int numLines = (subdivisions + 1) * 2;
	int vertexCount = numLines * 2;

	float corner = 0.5f;
	float step = 1.0f / static_cast<float>(subdivisions);

	int index = 0;
	float s = -corner;

	const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(1, 1, 1, 1);

	// Create vertical lines
	float scaling = static_cast<float>(subdivisions) * scale;
	DirectX::XMMATRIX M = DirectX::XMMatrixScaling(scaling, scaling, scaling);
	DirectX::XMVECTOR V, P;
	Vector3 position;
	for (int i = 0; i <= subdivisions; i++)
	{
		V = DirectX::XMVectorSet(s, 0, corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position);

		V = DirectX::XMVectorSet(s, 0, -corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position);

		s += step;
	}

	// Create horizontal lines
	s = -corner;
	for (int i = 0; i <= subdivisions; i++)
	{
		V = DirectX::XMVectorSet(corner, 0, s, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position);

		V = DirectX::XMVectorSet(-corner, 0, s, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position);

		s += step;
	}

	// 軸描画
	DrawAxis({
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1,
		});
}

void DebugRenderer::AddVertex(const Vector3& position)
{
	Vector3& v = _gridVertices.emplace_back();
	v = position;
}

// メッシュ生成
void DebugRenderer::CreateMesh(ID3D11Device* device,
	const std::vector<Vector3>& vertices, Mesh& mesh)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = static_cast<UINT>(sizeof(Vector3) * vertices.size());
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices.data();
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	mesh.vertexCount = static_cast<UINT>(vertices.size());
}

// 箱メッシュ作成
void DebugRenderer::CreateBoxMesh(ID3D11Device* device,
	float width, float height, float depth)
{
	DirectX::XMFLOAT3 positions[8] =
	{
		// top
		{ -width,  height, -depth},
		{  width,  height, -depth},
		{  width,  height,  depth},
		{ -width,  height,  depth},
		// bottom
		{ -width, -height, -depth},
		{  width, -height, -depth},
		{  width, -height,  depth},
		{ -width, -height,  depth},
	};

	std::vector<Vector3> vertices;
	vertices.resize(32);

	// top
	vertices.emplace_back(positions[0]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[0]);
	// bottom
	vertices.emplace_back(positions[4]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[6]);
	vertices.emplace_back(positions[6]);
	vertices.emplace_back(positions[7]);
	vertices.emplace_back(positions[7]);
	vertices.emplace_back(positions[4]);
	// side
	vertices.emplace_back(positions[0]);
	vertices.emplace_back(positions[4]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[6]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[7]);

	// メッシュ生成
	CreateMesh(device, vertices, _boxMesh);
}

// 球メッシュ作成
void DebugRenderer::CreateSphereMesh(ID3D11Device* device, float radius, int subdivisions)
{
	float step = DirectX::XM_2PI / subdivisions;

	std::vector<Vector3> vertices;

	// XZ平面
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = step * ((i + j) % subdivisions);

			Vector3& p = vertices.emplace_back();
			p.x = sinf(theta) * radius;
			p.y = 0.0f;
			p.z = cosf(theta) * radius;
		}
	}
	// XY平面
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = step * ((i + j) % subdivisions);

			Vector3& p = vertices.emplace_back();
			p.x = sinf(theta) * radius;
			p.y = cosf(theta) * radius;
			p.z = 0.0f;
		}
	}
	// YZ平面
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& p = vertices.emplace_back();
			p.x = 0.0f;
			p.y = sinf(theta) * radius;
			p.z = cosf(theta) * radius;
		}
	}

	// メッシュ生成
	CreateMesh(device, vertices, _sphereMesh);
}

// 半球メッシュ作成
void DebugRenderer::CreateHalfSphereMesh(ID3D11Device* device, float radius, int subdivisions)
{
	std::vector<Vector3> vertices;

	float theta_step = DirectX::XM_2PI / subdivisions;

	// XZ平面
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			Vector3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius;
			v.y = 0.0f;
			v.z = cosf(theta) * radius;
		}
	}
	// XY平面
	for (int i = 0; i < subdivisions / 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions) - DirectX::XM_PIDIV2;

			Vector3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius;
			v.y = cosf(theta) * radius;
			v.z = 0.0f;
		}
	}
	// YZ平面
	for (int i = 0; i < subdivisions / 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			Vector3& v = vertices.emplace_back();

			v.x = 0.0f;
			v.y = sinf(theta) * radius;
			v.z = cosf(theta) * radius;
		}
	}

	// メッシュ生成
	CreateMesh(device, vertices, _halfSphereMesh);
}

// 円柱
void DebugRenderer::CreateCylinderMesh(ID3D11Device* device,
	float radius1, float radius2, float start, float height, int subdivisions)
{
	std::vector<Vector3> vertices;

	float theta_step = DirectX::XM_2PI / subdivisions;

	// XZ平面
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			Vector3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius1;
			v.y = start;
			v.z = cosf(theta) * radius1;
		}
	}
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			Vector3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius2;
			v.y = start + height;
			v.z = cosf(theta) * radius2;
		}
	}
	// XY平面
	{
		vertices.emplace_back(Vector3(0.0f, start,			radius1));
		vertices.emplace_back(Vector3(0.0f, start + height, radius2));
		vertices.emplace_back(Vector3(0.0f, start,			-radius1));
		vertices.emplace_back(Vector3(0.0f, start + height, -radius2));
	}
	// YZ平面
	{
		vertices.emplace_back(Vector3(radius1, start,			0.0f));
		vertices.emplace_back(Vector3(radius2, start + height,	0.0f));
		vertices.emplace_back(Vector3(-radius1, start,			0.0f));
		vertices.emplace_back(Vector3(-radius2, start + height, 0.0f));
	}

	// メッシュ生成
	CreateMesh(device, vertices, _cylinderMesh);
}

// 骨メッシュ作成
void DebugRenderer::CreateBoneMesh(ID3D11Device* device, float length)
{
	float width = length * 0.25f;
	Vector3 positions[8] =
	{
		{ -0.00f,  0.00f,  0.00f},
		{  width,  0.00f,  width},
		{  0.00f,  0.00f,  length},
		{ -width,  0.00f,  width},
		{  0.00f,  width,  width},
		{  0.00f, -width,  width},
	};

	std::vector<Vector3> vertices;
	vertices.reserve(24);

	// xz
	vertices.emplace_back(positions[0]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[0]);
	// yz
	vertices.emplace_back(positions[0]);
	vertices.emplace_back(positions[4]);
	vertices.emplace_back(positions[4]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[0]);
	// xy
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[4]);
	vertices.emplace_back(positions[4]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[1]);

	// メッシュ生成
	CreateMesh(device, vertices, _boneMesh);
}

// 矢印メッシュ作成
void DebugRenderer::CreateArrowMesh(ID3D11Device* device)
{
	//		   1
	//			^
	//		  / | \
	//		 /  |  \
	//		 ---|---
	//			|
	// ---------|--------------------->
	//		   0

	std::vector<Vector3> vertices;
	// Y軸
	vertices.push_back({ 0,0,0 });
	vertices.push_back({ 0,1,0 });

	// X軸の三角
	vertices.push_back({ 0,1,0 });
	vertices.push_back({ 0.5f,0.5f,0 });
	vertices.push_back({ 0,1,0 });
	vertices.push_back({ -0.5f,0.5f,0 });

	vertices.push_back({ 0.5f,0.5f,0 });
	vertices.push_back({ -0.5f,0.5f,0 });

	// Z軸の三角
	vertices.push_back({ 0,1,0 });
	vertices.push_back({ 0,0.5f,0.5f });
	vertices.push_back({ 0,1,0 });
	vertices.push_back({ 0,0.5f,-0.5f });

	vertices.push_back({ 0,0.5f,0.5f });
	vertices.push_back({ 0,0.5f,-0.5f });


	vertices.push_back({ 0.5f,	0.5f,	0 });
	vertices.push_back({ 0,		0.5f,	0.5f });

	vertices.push_back({ 0,		0.5f,	0.5f });
	vertices.push_back({ -0.5f,	0.5f,	0 });

	vertices.push_back({ -0.5f,	0.5f,	0 });
	vertices.push_back({ 0,		0.5f,	-0.5f });

	vertices.push_back({ 0,		0.5f,	-0.5f });
	vertices.push_back({ 0.5f,	0.5f,	0 });


	// メッシュ生成
	CreateMesh(device, vertices, _arrowMesh);
}

// 軸メッシュ作成
void DebugRenderer::CreateAxis(ID3D11Device* device)
{
	// X軸
	{
		std::vector<Vector3> vertices;
		vertices.push_back({ 0,0,0 });
		vertices.push_back({ 1,0,0 });
		// メッシュ生成
		CreateMesh(device, vertices, _axisMesh[0]);
	}
	// Y軸
	{
		std::vector<Vector3> vertices;
		vertices.push_back({ 0,0,0 });
		vertices.push_back({ 0,1,0 });
		// メッシュ生成
		CreateMesh(device, vertices, _axisMesh[1]);
	}
	// Z軸
	{
		std::vector<Vector3> vertices;
		vertices.push_back({ 0,0,0 });
		vertices.push_back({ 0,0,1 });
		// メッシュ生成
		CreateMesh(device, vertices, _axisMesh[2]);
	}
}

// 描画実行
void DebugRenderer::Render(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{
	// シェーダー設定
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(_inputLayout.Get());

	// 定数バッファ設定
	dc->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

	// ビュープロジェクション行列作成
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX VP = V * P;

	// プリミティブ設定
	UINT stride = sizeof(Vector3);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// F7キーでキー非表示
#ifdef _DEBUG
	if (!Debug::Input::IsActive(DebugInput::BTN_F7))
	{
		for (const Instance& instance : _instances)
		{
			// 頂点バッファ設定
			dc->IASetVertexBuffers(0, 1, instance.mesh->vertexBuffer.GetAddressOf(), &stride, &offset);

			// ワールドビュープロジェクション行列作成
			DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&instance.worldTransform);
			DirectX::XMMATRIX WVP = W * VP;

			// 定数バッファ更新
			CbMesh cbMesh;
			DirectX::XMStoreFloat4x4(&cbMesh.worldViewProjection, WVP);
			cbMesh.color = instance.color;

			dc->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

			// 描画
			dc->Draw(instance.mesh->vertexCount, 0);
		}

		// グリッド描画
		{
			DirectX::XMMATRIX WVP = VP;
			// 定数バッファ更新
			CbMesh cbMesh;
			DirectX::XMStoreFloat4x4(&cbMesh.worldViewProjection, WVP);
			cbMesh.color = Vector4::White;
			dc->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
			// 頂点バッファ設定
			stride = sizeof(Vector3);
			offset = 0;
			dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			dc->IASetVertexBuffers(0, 1, _gridVertexBuffer.GetAddressOf(), &stride, &offset);

			UINT totalVertexCount = static_cast<UINT>(_gridVertices.size());
			UINT start = 0;
			UINT count = (totalVertexCount < VertexCapacity) ? totalVertexCount : VertexCapacity;

			while (start < totalVertexCount)
			{
				D3D11_MAPPED_SUBRESOURCE mappedSubresource;
				HRESULT hr = dc->Map(_gridVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				memcpy(mappedSubresource.pData, &_gridVertices[start], sizeof(Vector3) * count);

				dc->Unmap(_gridVertexBuffer.Get(), 0);

				dc->Draw(count, 0);

				start += count;
				if ((start + count) > totalVertexCount)
				{
					count = totalVertexCount - start;
				}
			}
		}
	}
#endif
	_gridVertices.clear();
	_instances.clear();
}
