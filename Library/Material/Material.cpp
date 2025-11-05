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
	static std::vector<const char*> shaderNames;
	for (size_t i = 0; i < static_cast<int>(ShaderType::Model); ++i)
	{
		shaderNames.push_back(ToString<ShaderType>(i).c_str());
	}

	ImGui::Text((u8"マテリアル名:" + _name).c_str());
	ImGui::Separator();

	int sId = static_cast<int>(_shaderType);
	if (ImGui::Combo(u8"シェーダータイプ", &sId, shaderNames.data(), (int)shaderNames.size()))
		SetShaderType(static_cast<ShaderType>(sId));
	ImGui::Separator();

	int bId = static_cast<int>(_blendType);
	if (ImGui::Combo(u8"ブレンドタイプ", &bId, blendTypeName, _countof(blendTypeName)))
		SetBlendType(static_cast<BlendType>(bId));
	ImGui::Separator();

	ImGui::Text(u8"使用中のシェーダー:");
	ImGui::SameLine();
	ImGui::Text(_shaderName.c_str());
	ImGui::Separator();

	// ColorMapGUI描画
	DrawColorMapGui();
	ImGui::Separator();

	// TextureMapGUI描画
	DrawTextureMapGui();
	ImGui::Separator();

	// ParameterMapGUI描画
	DrawParameterMapGui();
	ImGui::Separator();
}

/// ファイルからテクスチャを読み込む
void Material::LoadTexture(const std::string& key,
	const wchar_t* filename)
{
	// テクスチャの読み込み
	_textureDatas[key].Load(Graphics::Instance().GetDevice(), filename);
}

/// ダミーテクスチャ作成
void Material::MakeDummyTexture(const std::string& key,
	DWORD value, 
	UINT dimension)
{
	// テクスチャの作成
	_textureDatas[key].MakeDummyTexture(Graphics::Instance().GetDevice(), value, dimension);
}

// ColorMapGUI描画
void Material::DrawColorMapGui()
{
	for (auto& [key, color] : _colors)
	{
		ImGui::ColorEdit4(key.c_str(), &color.x);
	}
}

// TextureMapGUI描画
void Material::DrawTextureMapGui()
{
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

// ParameterMapGUI描画
void Material::DrawParameterMapGui()
{
	for (auto& [key, parameter] : _parameters)
	{
		if (std::holds_alternative<int>(parameter))
		{
			int* value = std::get_if<int>(&parameter);
			if (value)
				ImGui::DragInt(key.c_str(), value);
		}
		else if (std::holds_alternative<float>(parameter))
		{
			float* value = std::get_if<float>(&parameter);
			if (value)
				ImGui::DragFloat(key.c_str(), value, 0.1f);
		}
		else if (std::holds_alternative<Vector2>(parameter))
		{
			Vector2* value = std::get_if<Vector2>(&parameter);
			if (value)
				ImGui::DragFloat2(key.c_str(), &value->x, 0.1f);
		}
		else if (std::holds_alternative<Vector3>(parameter))
		{
			Vector3* value = std::get_if<Vector3>(&parameter);
			if (value)
				ImGui::DragFloat3(key.c_str(), &value->x, 0.1f);
		}
		else if (std::holds_alternative<Vector4>(parameter))
		{
			Vector4* value = std::get_if<Vector4>(&parameter);
			if (value)
				ImGui::DragFloat4(key.c_str(), &value->x, 0.1f);
		}
	}
}
