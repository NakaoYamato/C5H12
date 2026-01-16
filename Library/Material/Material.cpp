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
	static std::vector<char> shaderNames;
	if (shaderNames.empty())
	{
		for (size_t i = 0; i < static_cast<int>(ShaderType::ShaderTypeMax); ++i)
		{
			for (auto& name : ToString<ShaderType>(i))
			{
				shaderNames.push_back(name);
			}
			shaderNames.push_back('\0');
		}
	}

	ImGui::Text((u8"マテリアル名:" + _name).c_str());
	ImGui::Separator();

	int sId = static_cast<int>(_shaderType);
	if (ImGui::Combo(u8"シェーダータイプ", &sId, shaderNames.data(), static_cast<int>(ShaderType::ShaderTypeMax)))
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

#pragma region 入出力
bool Material::LoadFromFile(nlohmann::json_abi_v3_12_0::json& json)
{
	auto& sub = json["Material"];
	_name = sub["Name"].get<std::string>();
	_shaderType = static_cast<ShaderType>(sub["ShaderType"].get<int>());
	_shaderName = sub["ShaderName"].get<std::string>();
	_blendType = static_cast<BlendType>(sub["BlendType"].get<int>());
	for (auto& item : sub["Colors"])
	{
		std::string key = item["Key"].get<std::string>();
		Vector4 color;
		color.x = item["ValueX"].get<float>();
		color.y = item["ValueY"].get<float>();
		color.z = item["ValueZ"].get<float>();
		color.w = item["ValueW"].get<float>();
		_colors[key] = color;
	}
	for (auto& item : sub["TextureDatas"])
	{
		std::string key = item["Key"].get<std::string>();
		std::string filepath = item["Filepath"].get<std::string>();
		_textureDatas[key].Load(Graphics::Instance().GetDevice(), ToWString(filepath).c_str());
	}
	for (auto& item : sub["Parameters"])
	{
		std::string key = item["Key"].get<std::string>();
		std::string type = item["Type"].get<std::string>();
		if (type == "int")
		{
			int value = item["Value"].get<int>();
			_parameters[key] = value;
		}
		else if (type == "float")
		{
			float value = item["Value"].get<float>();
			_parameters[key] = value;
		}
		else if (type == "Vector2")
		{
			Vector2 value;
			value.x = item["ValueX"].get<float>();
			value.y = item["ValueY"].get<float>();
			_parameters[key] = value;
		}
		else if (type == "Vector3")
		{
			Vector3 value;
			value.x = item["ValueX"].get<float>();
			value.y = item["ValueY"].get<float>();
			value.z = item["ValueZ"].get<float>();
			_parameters[key] = value;
		}
		else if (type == "Vector4")
		{
			Vector4 value;
			value.x = item["ValueX"].get<float>();
			value.y = item["ValueY"].get<float>();
			value.z = item["ValueZ"].get<float>();
			value.w = item["ValueW"].get<float>();
			_parameters[key] = value;
		}
	}
	return true;
}

bool Material::SaveToFile(nlohmann::json_abi_v3_12_0::json& json)
{
	auto& sub = json["Material"];
	sub["Name"] = _name;
	sub["ShaderType"] = static_cast<int>(_shaderType);
	sub["ShaderName"] = _shaderName;
	sub["BlendType"] = static_cast<int>(_blendType);
	size_t index = 0;
	for (auto& [key, color] : _colors)
	{
		sub["Colors"][index]["Key"] = key;
		sub["Colors"][index]["ValueX"] = color.x;
		sub["Colors"][index]["ValueY"] = color.y;
		sub["Colors"][index]["ValueZ"] = color.z;
		sub["Colors"][index]["ValueW"] = color.w;
		++index;
	}
	index = 0;
	for (auto& [key, textureData] : _textureDatas)
	{
		sub["TextureDatas"][index]["Key"] = key;
		sub["TextureDatas"][index]["Filepath"] = textureData.GetFilepath();
		++index;
	}
	index = 0;
	for (auto& [key, parameter] : _parameters)
	{
		sub["Parameters"][index]["Key"] = key;
		if (std::holds_alternative<int>(parameter))
		{
			sub["Parameters"][index]["Type"] = "int";
			sub["Parameters"][index]["Value"] = std::get<int>(parameter);
		}
		else if (std::holds_alternative<float>(parameter))
		{
			sub["Parameters"][index]["Type"] = "float";
			sub["Parameters"][index]["Value"] = std::get<float>(parameter);
		}
		else if (std::holds_alternative<Vector2>(parameter))
		{
			sub["Parameters"][index]["Type"] = "Vector2";
			auto& value = std::get<Vector2>(parameter);
			sub["Parameters"][index]["ValueX"] = value.x;
			sub["Parameters"][index]["ValueY"] = value.y;
		}
		else if (std::holds_alternative<Vector3>(parameter))
		{
			sub["Parameters"][index]["Type"] = "Vector3";
			auto& value = std::get<Vector3>(parameter);
			sub["Parameters"][index]["ValueX"] = value.x;
			sub["Parameters"][index]["ValueY"] = value.y;
			sub["Parameters"][index]["ValueZ"] = value.z;
		}
		else if (std::holds_alternative<Vector4>(parameter))
		{
			sub["Parameters"][index]["Type"] = "Vector4";
			auto& value = std::get<Vector4>(parameter);
			sub["Parameters"][index]["ValueX"] = value.x;
			sub["Parameters"][index]["ValueY"] = value.y;
			sub["Parameters"][index]["ValueZ"] = value.z;
			sub["Parameters"][index]["ValueW"] = value.w;
		}
		++index;
	}

	return true;
}
#pragma endregion

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
		ImGui::Text(u8"サイズ:");
		ImGui::SameLine();
		ImGui::Text("x : %f y : %f", textureData.GetTextureSize().x, textureData.GetTextureSize().y);
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
