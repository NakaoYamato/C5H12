#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "../../Library/Math/Vector.h"
#include "../../Library/Math/Quaternion.h"

#include "../../Library/Graphics/ConstantBuffer.h"
#include "../../Library/Material/Material.h"
#include "../../Library/Shader/Primitive/PrimitiveShaderBase.h"

class PrimitiveRenderer
{
public:
	static const UINT VertexCapacity = 3 * 1024;

	static const UINT CBIndex = 1;
	struct CbPrimitive
	{
		UINT	vertexCount;	// 頂点数
		Vector2 viewportSize;	// ビューポートのサイズ
		float	padding;		// パディング用
	};

	struct Vertex
	{
		Vector3	position;
		Vector4	color;
	};

	struct RenderInfo
	{
		Material*			material = nullptr;
		std::vector<Vertex>	vertices;
	};

public:
	PrimitiveRenderer() = default;
	~PrimitiveRenderer() = default;

	// 初期化処理
	void Initialize(ID3D11Device* device);

	// 描画
	void Draw(const RenderInfo& info);

	// 描画実行
	void Render(RenderContext& rc);

	Material::ParameterMap GetParameterMap(const std::string& shaderName)const
	{
		if (_shaders.find(shaderName) != _shaders.end())
			return _shaders.at(shaderName)->GetParameterMap();
		return Material::ParameterMap{};
	}

private:
	std::vector<RenderInfo>						_renderInfos;
	// 描画登録時の排他制御用
	std::mutex									_drawInfoMutex;

	Microsoft::WRL::ComPtr<ID3D11Buffer>		_vertexBuffer;
	ConstantBuffer 								_constantBuffer;
	std::unordered_map<std::string, std::unique_ptr<PrimitiveShaderBase>> _shaders;
};
