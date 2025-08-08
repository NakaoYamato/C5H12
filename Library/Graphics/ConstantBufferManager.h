#pragma once
#include <d3d11_1.h>
#include <DirectXMath.h>

#include <memory>
#include <wrl.h>
#include <unordered_map>
#include <string>

#include "RenderContext.h"

/// <summary>
/// 定数バッファの種類
/// </summary>
enum class ConstantBufferType
{
	SceneCB,
	LightCB,

	CBtypeMax,
};

/// <summary>
/// 定数バッファを更新する対象
/// </summary>
enum class ConstantUpdateTarget : int
{
	Vertex		= 0b000001,
	Hull		= 0b000010,
	Domain		= 0b000100,
	Geometory	= 0b001000,
	Pixel		= 0b010000,
	Compute		= 0b100000,
	ALL			= 0b111111,
};

/// <summary>
/// 定数バッファの管理者
/// </summary>
class ConstantBufferManager
{
public:
	struct ConstantBufferBase {};

	// シーン定数バッファ
	struct SceneConstantBuffer : public ConstantBufferBase
	{
		DirectX::XMFLOAT4X4		view;
		DirectX::XMFLOAT4X4		projection;
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4X4		invView;
		DirectX::XMFLOAT4X4		invProjection;
		DirectX::XMFLOAT4X4		invViewProjection;
		DirectX::XMFLOAT4		lightDirection;
		DirectX::XMFLOAT4		lightColor;
		DirectX::XMFLOAT4		cameraPosition;

		float					aspect{};
		float					sumDistance = 1000.0f;
		DirectX::XMFLOAT2		sceneDummy{};

		float					totalElapsedTime{};
		float					deltaTime{};
		float					windFrequency = 22.388f;
		float					windStrength = 0.5f;
	};
	// ライト定数バッファ
	struct LightConstantBuffer : public ConstantBufferBase
	{
		DirectX::XMFLOAT4 ambientColor{};
		DirectX::XMFLOAT4 directionalLightDirection{};
		DirectX::XMFLOAT4 directionalLightColor{ 1,0,0,1 };
		PointLight pointLights[8];
	};

public:
	ConstantBufferManager(ID3D11Device* device);
	~ConstantBufferManager() {}

	// 定数バッファの一括更新
	void Update(const RenderContext& rc);

	/// <summary>
	/// 指定の定数バッファの更新
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="bufferType">更新する対象</param>
	/// <param name="parameter">更新する値 関数内でダイナミックキャスト</param>
	void UpdateCB(ID3D11DeviceContext* dc,
		ConstantBufferType bufferType,
		ConstantBufferBase* parameter);

	/// <summary>
	/// 指定の定数バッファをセット
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="slot">スロット番号</param>
	/// <param name="bufferType">セットする対象</param>
	/// <param name="updateTarget">どのシェーダに対してセットするか</param>
	void SetCB(ID3D11DeviceContext* dc,
		UINT slot,
		ConstantBufferType bufferType,
		ConstantUpdateTarget updateTarget);

	SceneConstantBuffer& GetSceneCB() { return _sceneCB; }
	LightConstantBuffer& GetLightCB() { return _lightCB; }
private:
	// シーン定数バッファの更新
	void UpdateSceneCB(SceneConstantBuffer* buffer);
	// ライト定数バッファの更新
	void UpdateLightCB(LightConstantBuffer* buffer);

private:
	// 定数バッファ
	SceneConstantBuffer _sceneCB{};
	LightConstantBuffer _lightCB{};

	Microsoft::WRL::ComPtr<ID3D11Buffer>	_constantBuffers[static_cast<size_t>(ConstantBufferType::CBtypeMax)];
};