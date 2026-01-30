#include "SpriteShaderResource.h"

#include "Simple/SpriteSimpleShader.h"
#include "Scroll/SpriteScrollShader.h"
#include "Outline/SpriteOutlineShader.h"
#include "Dissolve/SpriteDissolveShader.h"

#include <imgui.h>

// 初期化処理
void SpriteShaderResource::Initialize(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	_blitVertexShader.Load(device, "./Data/Shader/HLSL/Sprite/FullscreenQuadVS.cso",
		input_element_desc, _countof(input_element_desc));
	_blitPixelShader.Load(device, "./Data/Shader/HLSL/Sprite/FullscreenQuadPS.cso");

	// シェーダー作成
	{
		_shaders["Simple"] = std::make_unique<SpriteSimpleShader>(device,
			input_element_desc, static_cast<UINT>(_countof(input_element_desc)));
		_shaders["Scroll"] = std::make_unique<SpriteScrollShader>(device,
			input_element_desc, static_cast<UINT>(_countof(input_element_desc)));
		_shaders["Outline"] = std::make_unique<SpriteOutlineShader>(device,
			input_element_desc, static_cast<UINT>(_countof(input_element_desc)));
		_shaders["Dissolve"] = std::make_unique<SpriteDissolveShader>(device,
			input_element_desc, static_cast<UINT>(_countof(input_element_desc)));
	}

}

// Gui描画
void SpriteShaderResource::DrawGui()
{
}

// マテリアルの編集GUI表示
void SpriteShaderResource::DrawMaterialEditGui(Material* material)
{
	// 使用可能なシェーダー取得
	auto activeShaderTypes = GetShaderNames();
	// シェーダー変更GUI
	if (ImGui::TreeNode(u8"シェーダー変更"))
	{
		const auto& shaderType = material->GetShaderName();
		for (auto& activeShaderType : activeShaderTypes)
		{
			bool active = activeShaderType == shaderType;
			if (ImGui::RadioButton(activeShaderType, active))
			{
				material->SetShaderName(activeShaderType);
				// シェーダー変更時はパラメータも初期化
				material->SetParameterMap(GetShaderParameterKey(activeShaderType));
			}
		}
		ImGui::TreePop();
	}
}
