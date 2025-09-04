#include "Material.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Algorithm/Converter.h"

#include "../../Library/DebugSupporter/DebugSupporter.h"
#include <filesystem>
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
		ImGui::PushID(&textureData.filename);
		ImGui::SameLine();
		if (ImGui::Button("..."))
		{
			// ダイアログを開く
			std::string filepath;
			std::string currentDirectory;
			const char* filter = "Texture Files(*.dds;*.png;*.tga;*.jpg;*.tif)\0*.dds;*.png;*.tga;*.jpg;*.tif;\0All Files(*.*)\0*.*;\0\0";
			Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
            // ファイルを選択したら
            if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
			{
				// 相対パス取得
				std::filesystem::path path =
					std::filesystem::relative(filepath, currentDirectory);
                // テクスチャの読み込み
                textureData.filename = path.c_str();
				GpuResourceManager::LoadTextureFromFile(Graphics::Instance().GetDevice(),
					textureData.filename.c_str(),
					textureData.textureSRV.ReleaseAndGetAddressOf(),
					&textureData.texture2dDesc);
			}
		}
		ImGui::PopID();
		ImGui::Separator();
	}
}

/// ファイルからテクスチャを読み込む
void Material::LoadTexture(const std::string& key,
	const wchar_t* filename)
{
	// テクスチャの読み込み
	TextureData& textureData = _textureDatas[key];
	textureData.filename = filename;
	bool res = GpuResourceManager::LoadTextureFromFile(Graphics::Instance().GetDevice(), filename,
		textureData.textureSRV.ReleaseAndGetAddressOf(),
		&textureData.texture2dDesc);

    // 読み込みに失敗した場合はダミーテクスチャを作成
	if (!res)
	{
		Debug::Output::String(L"テクスチャの読み込みに失敗しました: ");
		Debug::Output::String(filename);
		Debug::Output::String(L"\n");
		// 赤色の1x1ダミーテクスチャを作成
		MakeDummyTexture(key, 0xFF0000FF, 1);
	}
}

/// ダミーテクスチャ作成
void Material::MakeDummyTexture(const std::string& key,
	DWORD value, 
	UINT dimension)
{
	// テクスチャの作成
	TextureData& textureData = _textureDatas[key];
	GpuResourceManager::MakeDummyTexture(Graphics::Instance().GetDevice(),
		textureData.textureSRV.ReleaseAndGetAddressOf(), 
		&textureData.texture2dDesc,
		value, dimension);
}
