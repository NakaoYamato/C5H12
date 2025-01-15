#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "../../Library/Math/Vector.h"

class DebugRenderer
{
public:
	DebugRenderer(ID3D11Device* device);
	~DebugRenderer() {}

	// 箱描画
	void DrawBox(
		const Vector3& position,
		const Vector3& angle,
		const Vector3& size,
		const Vector4& color);
	void DrawBox(
		const DirectX::XMFLOAT4X4& transform,
		const Vector4& color);

	// 球描画
	void DrawSphere(
		const Vector3& position,
		float radius,
		const Vector4& color);

	// カプセル描画
	void DrawCapsule(
		const DirectX::XMFLOAT4X4& transform,
		float radius,
		float height,
		const Vector4& color);
	// カプセル描画
	void DrawCapsule(
		const Vector3& start,
		const Vector3& end,
		float radius,
		const Vector4& color);

	// 骨描画
	void DrawBone(
		const DirectX::XMFLOAT4X4& transform,
		float length,
		const Vector4& color);

	// 矢印描画
	void DrawArrow(
		const Vector3& start,
		const Vector3& target,
		float radius,
		const Vector4& color);

	// 軸描画
	void DrawAxis(const DirectX::XMFLOAT4X4& transform);

	// グリッド描画
	void DrawGrid(int subdivisions, float scale);

	// 頂点追加
	void AddVertex(const Vector3& position);

	// 描画実行
	void Render(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

private:
	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
		UINT									vertexCount{};
	};

	struct Instance
	{
		Mesh* mesh = nullptr;
		DirectX::XMFLOAT4X4		worldTransform{};
		Vector4		color{ 1,1,1,1 };
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4		worldViewProjection{};
		Vector4		color{ 1,1,1,1 };
	};

	// メッシュ生成
	void CreateMesh(ID3D11Device* device, const std::vector<Vector3>& vertices, Mesh& mesh);

	// 箱メッシュ作成
	void CreateBoxMesh(ID3D11Device* device, float width, float height, float depth);

	// 球メッシュ作成
	void CreateSphereMesh(ID3D11Device* device, float radius, int subdivisions);

	// 半球メッシュ作成
	void CreateHalfSphereMesh(ID3D11Device* device, float radius, int subdivisions);

	// 円柱
	void CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int subdivisions);

	// 骨メッシュ作成
	void CreateBoneMesh(ID3D11Device* device, float length);

	// 矢印メッシュ作成
	void CreateArrowMesh(ID3D11Device* device);

	// 軸メッシュ作成
	void CreateAxis(ID3D11Device* device);

private:
	Mesh										boxMesh;
	Mesh										sphereMesh;
	Mesh										halfSphereMesh;
	Mesh										cylinderMesh;
	Mesh										boneMesh;
	Mesh										arrowMesh;
	Mesh										axisMesh[3];// 0:X, 1:Y, 2:Z
	std::vector<Instance>						instances;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer;

	// グリッド描画用
	static const UINT VertexCapacity = 3 * 1024;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	gridVertexBuffer;
	std::vector<Vector3> gridVertices;
};
