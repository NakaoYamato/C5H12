#include "TerrainDeformationResource.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Library/Exporter/Exporter.h"

#include <Mygui.h>

// ペイントテクスチャのデフォルトデータパス
static const char* DEFAULT_PAINT_TEXTURE_PATH = "./Data/Terrain/Debug/DefaultPaintTexture.json";
// ブラシテクスチャのデフォルトデータパス
static const char* DEFAULT_BRUSH_TEXTURE_PATH = "./Data/Terrain/Debug/DefaultBrushTexture.json";
// 配置するモデルデータのデフォルトデータパス
static const char* DEFAULT_MODEL_DATA_PATH = "./Data/Terrain/Debug/DefaultModelData.json";

// 初期化処理
bool TerrainDeformationResource::Initialize()
{
	// ペイントテクスチャデータ構築
	BuildPaintTextureResources();
	// ブラシテクスチャデータ構築
	BuildBrushTextureResources();

	_isInitialized = true;
	return true;
}

// ファイル読み込み
bool TerrainDeformationResource::LoadFromFile()
{
	// ペイントテクスチャデータ読み込み
	LoadPaintTextures();
	// ブラシテクスチャデータ読み込み
	LoadBrushTextures();
	// 配置するモデルデータ読み込み
	LoadModelData();

	// 初期化が完了しているならこの場でリソース構築
	if (_isInitialized)
	{
		// ペイントテクスチャデータ構築
		BuildPaintTextureResources();
		// ブラシテクスチャデータ構築
		BuildBrushTextureResources();
	}

	return true;
}

// ファイル保存
bool TerrainDeformationResource::SaveToFile()
{
	// ペイントテクスチャデータ書き出し
	SavePaintTextures();
	// ブラシテクスチャデータ書き出し
	SaveBrushTextures();
	// 配置するモデルデータ書き出し
	SaveModelData();

	return true;
}

// Gui描画
void TerrainDeformationResource::DrawGui()
{
	if (ImGui::TreeNode(u8"ペイントテクスチャ"))
	{
		// ペイントテクスチャのGUI描画
		DrawPaintTextureGui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"ブラシテクスチャ"))
	{
		// ブラシテクスチャのGUI描画
		DrawBrushTextureGui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"配置するモデルデータ"))
	{
		// モデルの選択GUI描画
		DrawModelSelectionGui();
		ImGui::TreePop();
	}
}

#pragma region 読み込み
// ペイントテクスチャデータ読み込み
bool TerrainDeformationResource::LoadPaintTextures()
{
    nlohmann::json jsonData;
    if (Exporter::LoadJsonFile(DEFAULT_PAINT_TEXTURE_PATH, &jsonData))
    {
        size_t size = jsonData["Size"].get<std::size_t>();
        for (size_t i = 0; i < size; ++i)
        {
            std::string key{};
			PaintTexture& tex = _paintTextures.emplace_back();

            key = "ColorPath" + std::to_string(i);
            if (jsonData.contains(key))
				tex.baseColorPath = jsonData[key].get<std::wstring>();
            key = "NormalPath" + std::to_string(i);
            if (jsonData.contains(key))
				tex.normalPath = jsonData[key].get<std::wstring>();
            key = "HeightPath" + std::to_string(i);
            if (jsonData.contains(key))
				tex.heightPath = jsonData[key].get<std::wstring>();
        }

		return true;
    }
	return false;
}

// ブラシテクスチャデータ読み込み
bool TerrainDeformationResource::LoadBrushTextures()
{
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(DEFAULT_BRUSH_TEXTURE_PATH, &jsonData))
	{
		size_t size = jsonData["Size"].get<std::size_t>();
		for (size_t i = 0; i < size; ++i)
		{
			BrushTexture& tex = _brushTextures.emplace_back();
			tex.path = jsonData["Path" + std::to_string(i)].get<std::wstring>();
		}

		return true;
	}
	return false;
}

// 配置するモデルデータ読み込み
bool TerrainDeformationResource::LoadModelData()
{
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(DEFAULT_MODEL_DATA_PATH, &jsonData))
	{
		size_t size = jsonData["Size"].get<std::size_t>();
		for (size_t i = 0; i < size; ++i)
		{
			ModelData& modelData = _environmentObjects.emplace_back();
			modelData.path = jsonData["ModelPath" + std::to_string(i)].get<std::string>();
		}

		return true;
	}
	return false;
}
#pragma endregion

#pragma region 書き出し
// ペイントテクスチャデータ書き出し
bool TerrainDeformationResource::SavePaintTextures()
{
	nlohmann::json jsonData;
	jsonData["Size"] = _paintTextures.size();
	for (size_t i = 0; i < _paintTextures.size(); ++i)
	{
		jsonData["ColorPath" + std::to_string(i)] = _paintTextures[i].baseColorPath;
		jsonData["NormalPath" + std::to_string(i)] = _paintTextures[i].normalPath;
		jsonData["HeightPath" + std::to_string(i)] = _paintTextures[i].heightPath;
	}
	return Exporter::SaveJsonFile(DEFAULT_PAINT_TEXTURE_PATH, jsonData);
}

// ブラシテクスチャデータ書き出し
bool TerrainDeformationResource::SaveBrushTextures()
{
	nlohmann::json jsonData;
	jsonData["Size"] = _brushTextures.size();
	for (size_t i = 0; i < _brushTextures.size(); ++i)
	{
		jsonData["Path" + std::to_string(i)] = _brushTextures[i].path;
	}
	return Exporter::SaveJsonFile(DEFAULT_BRUSH_TEXTURE_PATH, jsonData);
}

// 配置するモデルデータ書き出し
bool TerrainDeformationResource::SaveModelData()
{
	nlohmann::json jsonData;
	jsonData["Size"] = _environmentObjects.size();
	for (size_t i = 0; i < _environmentObjects.size(); ++i)
	{
		jsonData["ModelPath" + std::to_string(i)] = _environmentObjects[i].path;
	}
	return Exporter::SaveJsonFile(DEFAULT_MODEL_DATA_PATH, jsonData);
}
#pragma endregion

#pragma region 構築
// ペイントテクスチャデータ構築
bool TerrainDeformationResource::BuildPaintTextureResources()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	for (auto& tex : _paintTextures)
	{
		// ベースカラーの読み込み
		GpuResourceManager::LoadTextureFromFile(
			device,
			tex.baseColorPath.c_str(),
			tex.baseColorSRV.ReleaseAndGetAddressOf(),
			nullptr);
		// 法線マップの読み込み
		GpuResourceManager::LoadTextureFromFile(
			device,
			tex.normalPath.c_str(),
			tex.normalSRV.ReleaseAndGetAddressOf(),
			nullptr);
		// 高さマップの読み込み
		GpuResourceManager::LoadTextureFromFile(
			device,
			tex.heightPath.c_str(),
			tex.heightSRV.ReleaseAndGetAddressOf(),
			nullptr);
	}
	return true;
}

// ブラシテクスチャデータ構築
bool TerrainDeformationResource::BuildBrushTextureResources()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	for (auto& tex : _brushTextures)
	{
		// テクスチャの読み込み
		GpuResourceManager::LoadTextureFromFile(
			device,
			tex.path.c_str(),
			tex.textureSRV.ReleaseAndGetAddressOf(),
			nullptr);
	}
	return true;
}
#pragma endregion

#pragma region GUI
// ペイントテクスチャのGUI描画
void TerrainDeformationResource::DrawPaintTextureGui()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	if (ImGui::Button(u8"ペイントテクスチャ追加"))
	{
		try
		{
			std::wstring colorPath{}, normalPath{}, heightPath{};

			// ベースカラーのパスを取得するダイアログを開く
			std::string filepath;
			std::string currentDirectory;
			Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, ImGui::TextureFilter);
			if (result == Debug::Dialog::DialogResult::No || result == Debug::Dialog::DialogResult::Cancel)
				throw std::exception();
			colorPath = std::filesystem::relative(filepath, currentDirectory).wstring();

			// 法線マップのパスを取得するダイアログを開く
			result = Debug::Dialog::OpenFileName(filepath, currentDirectory, ImGui::TextureFilter);
			if (result == Debug::Dialog::DialogResult::No || result == Debug::Dialog::DialogResult::Cancel)
				throw std::exception();
			normalPath = std::filesystem::relative(filepath, currentDirectory).wstring();

			// 高さマップのパスを取得するダイアログを開く
			result = Debug::Dialog::OpenFileName(filepath, currentDirectory, ImGui::TextureFilter);
			if (result == Debug::Dialog::DialogResult::No || result == Debug::Dialog::DialogResult::Cancel)
				throw std::exception();
			heightPath = std::filesystem::relative(filepath, currentDirectory).wstring();

			{
				PaintTexture& tex = _paintTextures.emplace_back();
				tex.baseColorPath = colorPath;
				tex.normalPath = normalPath;
				tex.heightPath = heightPath;
				// テクスチャの読み込み
				GpuResourceManager::LoadTextureFromFile(
					device,
					tex.baseColorPath.c_str(),
					tex.baseColorSRV.ReleaseAndGetAddressOf(),
					nullptr);
				GpuResourceManager::LoadTextureFromFile(
					device,
					tex.normalPath.c_str(),
					tex.normalSRV.ReleaseAndGetAddressOf(),
					nullptr);
				GpuResourceManager::LoadTextureFromFile(
					device,
					tex.heightPath.c_str(),
					tex.heightSRV.ReleaseAndGetAddressOf(),
					nullptr);
			}
		}
		catch (const std::exception&)
		{
		}
	}
	ImGui::Separator();

	for (size_t i = 0; i < _paintTextures.size(); ++i)
	{
		// ベースカラーテクスチャの編集
		if (ImGui::ImageEditButton(&_paintTextures[i].baseColorPath, _paintTextures[i].baseColorSRV.Get()))
		{
			// テクスチャの読み込み
			GpuResourceManager::LoadTextureFromFile(
				device,
				_paintTextures[i].baseColorPath.c_str(),
				_paintTextures[i].baseColorSRV.ReleaseAndGetAddressOf(),
				nullptr);
		}
		ImGui::SameLine();
		// 法線マップの編集
		if (ImGui::ImageEditButton(&_paintTextures[i].normalPath, _paintTextures[i].normalSRV.Get()))
		{
			// テクスチャの読み込み
			GpuResourceManager::LoadTextureFromFile(
				device,
				_paintTextures[i].normalPath.c_str(),
				_paintTextures[i].normalSRV.ReleaseAndGetAddressOf(),
				nullptr);
		}
		ImGui::SameLine();
		// 高さマップの編集
		if (ImGui::ImageEditButton(&_paintTextures[i].heightPath, _paintTextures[i].heightSRV.Get()))
		{
			// テクスチャの読み込み
			GpuResourceManager::LoadTextureFromFile(
				device,
				_paintTextures[i].heightPath.c_str(),
				_paintTextures[i].heightSRV.ReleaseAndGetAddressOf(),
				nullptr);
		}
		ImGui::SameLine();
		// テクスチャの削除ボタン
		ImGui::PushID(static_cast<int>(i));
		if (ImGui::Button(u8"削除"))
		{
			// 選択中のペイントテクスチャを削除
			_paintTextures.erase(_paintTextures.begin() + i);
			ImGui::PopID();
			break; // 削除後はループを抜ける
		}
		ImGui::PopID();

		ImGui::Separator();
	}
}
// ブラシテクスチャのGUI描画
void TerrainDeformationResource::DrawBrushTextureGui()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	for (size_t i = 0; i < _brushTextures.size(); ++i)
	{
		if (ImGui::ImageEditButton(&_brushTextures[i].path, _brushTextures[i].textureSRV.Get()))
		{
			// テクスチャの読み込み
			GpuResourceManager::LoadTextureFromFile(
				device,
				_brushTextures[i].path.c_str(),
				_brushTextures[i].textureSRV.ReleaseAndGetAddressOf(),
				nullptr);
		}
	}
}
// モデルの選択GUI描画
void TerrainDeformationResource::DrawModelSelectionGui()
{
	for (size_t i = 0; i < _environmentObjects.size(); ++i)
	{
		ImGui::Text(u8"モデル %d: %s", static_cast<int>(i), _environmentObjects[i].path.c_str());
	}
}
#pragma endregion
