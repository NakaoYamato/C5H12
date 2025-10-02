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
		ImGui::Text(ToString(textureData.GetFilepath()).c_str());
		static float textureSize = 128.0f;
		ImGui::Image(textureData.Get(), { textureSize,textureSize });
		ImGui::PushID(textureData.GetFilepath().data());
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
				textureData.Load(Graphics::Instance().GetDevice(), path.c_str());
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
	_textureDatas[key].Load(Graphics::Instance().GetDevice(), filename);

	//_parameters["Test"].emplace<float>(1.0f);
	//_parameters["Test1"].emplace<Vector2>(Vector2(0,0));
	//if (auto* it = std::get_if<float>(&_parameters["Test"]))
	//	Debug::Output::String("float OK");
	//if (auto* it = std::get_if<float>(&_parameters["Test1"]))
	//	Debug::Output::String("float OK");
	//if (auto* it = std::get_if<Vector2>(&_parameters["Test1"]))
	//	Debug::Output::String("float OK");
}

/// ダミーテクスチャ作成
void Material::MakeDummyTexture(const std::string& key,
	DWORD value, 
	UINT dimension)
{
	// テクスチャの作成
	_textureDatas[key].MakeDummyTexture(Graphics::Instance().GetDevice(), value, dimension);
}
