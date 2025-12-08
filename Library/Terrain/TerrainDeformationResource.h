#pragma once

#include "../../Library/Resource/ResourceManager.h"
#include <d3d11.h>
#include <wrl.h>

class TerrainDeformationResource : public ResourceBase
{
public:
	// 書き込むテクスチャデータ
	struct PaintTexture
	{
		std::wstring baseColorPath; // ベースカラーのパス
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> baseColorSRV; // ベースカラーのSRV
		std::wstring normalPath;     // 法線マップのパス
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV; // 法線マップのSRV
		std::wstring heightPath;     // 高さマップのパス
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> heightSRV; // 高さマップのSRV
	};
	// ブラシテクスチャデータ
	struct BrushTexture
	{
		std::wstring path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	};
	// 配置するモデルデータ
	struct ModelData
	{
		std::string path = ""; // モデルのパス
	};

public:
	TerrainDeformationResource() = default;
	~TerrainDeformationResource() override {}

	// 初期化処理
	bool Initialize() override;

	// 名前取得
	std::string GetName() const override { return "TerrainDeformationResource"; }
	// ファイルパス取得
	std::string GetFilePath() const override { return ""; };
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;
	// Gui描画
	void DrawGui() override;

#pragma region アクセサ
	// ペイントテクスチャデータ取得
	const std::vector<PaintTexture>& GetPaintTextures() const { return _paintTextures; }
	// ブラシテクスチャデータ取得
	const std::vector<BrushTexture>& GetBrushTextures() const { return _brushTextures; }
	// 配置するモデルデータ取得
	const std::vector<ModelData>& GetEnvironmentObjects() const { return _environmentObjects; }
#pragma endregion

private:
#pragma region 読み込み
	// ペイントテクスチャデータ読み込み
	bool LoadPaintTextures();
	// ブラシテクスチャデータ読み込み
	bool LoadBrushTextures();
	// 配置するモデルデータ読み込み
	bool LoadModelData();
#pragma endregion

#pragma region 書き出し
	// ペイントテクスチャデータ書き出し
	bool SavePaintTextures();
	// ブラシテクスチャデータ書き出し
	bool SaveBrushTextures();
	// 配置するモデルデータ書き出し
	bool SaveModelData();
#pragma endregion

#pragma region 構築
	// ペイントテクスチャデータ構築
	bool BuildPaintTextureResources();
	// ブラシテクスチャデータ構築
	bool BuildBrushTextureResources();
#pragma endregion

#pragma region GUI
	// ペイントテクスチャのGUI描画
	void DrawPaintTextureGui();
	// ブラシテクスチャのGUI描画
	void DrawBrushTextureGui();
	// モデルの選択GUI描画
	void DrawModelSelectionGui();
#pragma endregion

private:
	// ペイントテクスチャデータ
	std::vector<PaintTexture> _paintTextures;
	// ブラシのテクスチャデータ
	std::vector<BrushTexture> _brushTextures;
	// 配置するモデルデータ
	std::vector<ModelData> _environmentObjects;

	bool _isInitialized = false;
};

// リソース設定
_REGISTER_RESOURCE(TerrainDeformationResource)
