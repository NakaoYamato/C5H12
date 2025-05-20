#include "Material.h"

#include "../../Library/ResourceManager/GpuResourceManager.h"
#include "../../Library/Renderer/MeshRenderer.h"

#include "../../Library/Algorithm/Converter.h"
#include <imgui.h>

void Material::DrawGui()
{
	static const char* blendTypeName[] =
	{
		u8"Opaque",
		u8"Alpha",
	};

	ImGui::Text((u8"マテリアル名:" + _name).c_str());

	ImGui::Separator();
	int bId = static_cast<int>(_blendType);
	if (ImGui::Combo(u8"ブレンドタイプ", &bId, blendTypeName, _countof(blendTypeName)))
		SetBlendType(static_cast<BlendType>(bId));
	ImGui::Separator();

	ImGui::Text(u8"使用中のシェーダー:");
	ImGui::SameLine();
	ImGui::Text(_shaderName.c_str());
	ImGui::Separator();

	for (auto& [key, color] : _colors)
	{
		ImGui::ColorEdit4(key.c_str(), &color.x);
	}
	ImGui::Separator();
	for (auto& [key, textureData] : _textureDatas)
	{
		ImGui::Text((u8"テクスチャ名:" + key).c_str());
		ImGui::Text(u8"ファイル名:");
		ImGui::SameLine();
		ImGui::Text(ToString(textureData.filename).c_str());
		static float textureSize = 128.0f;
		ImGui::Image(textureData.textureSRV.Get(), { textureSize,textureSize });
		ImGui::Separator();
	}
}

/// ファイルからテクスチャを読み込む
void Material::LoadTexture(ID3D11Device* device,
	const std::string& key,
	const wchar_t* filename)
{
	// テクスチャの読み込み
	TextureData textureData;
	textureData.filename = filename;
	GpuResourceManager::LoadTextureFromFile(device, filename,
		textureData.textureSRV.ReleaseAndGetAddressOf(),
		&textureData.texture2dDesc);
	// テクスチャデータを格納
	_textureDatas[key] = textureData;
}

/// ダミーテクスチャ作成
void Material::MakeDummyTexture(ID3D11Device* device, 
	const std::string& key,
	DWORD value, 
	UINT dimension)
{
	// テクスチャの作成
	TextureData textureData;
	GpuResourceManager::MakeDummyTexture(device,
		textureData.textureSRV.ReleaseAndGetAddressOf(), 
		&textureData.texture2dDesc,
		value, dimension);
	// テクスチャデータを格納
	_textureDatas[key] = textureData;
}
