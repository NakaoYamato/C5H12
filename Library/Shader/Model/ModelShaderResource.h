#pragma once

#include "ModelShaderBase.h"
#include "../ShaderResourceBase.h"

/// <summary>
/// モデルタイプ
/// </summary>
enum class ModelRenderType
{
	Dynamic,   // 骨の影響度がある(アニメーション等)
	Static,    // 骨の影響度がない(ステージ等のボーンがないモデル用)
	Instancing,// GPUインスタンシング使用

	ModelRenderTypeMax
};

class ModelShaderResource : public ShaderResourceBase
{
public:
	// シェーダーの配列
	using ShaderMap = std::unordered_map<std::string, std::unique_ptr<ModelShaderBase>>;

public:
	ModelShaderResource() = default;
	~ModelShaderResource() override {}

	// 名前取得
	std::string GetName() const override { return _name; }
	// ファイルパス取得
	std::string GetFilePath() const override { return _filePath; }
	// ファイル読み込み
	bool LoadFromFile() override { return false; }
	// ファイル保存
	bool SaveToFile() override { return false; }
	// シェーダータイプ取得
	ShaderType GetShaderType() const override { return ShaderType::Model; }

	// 初期化処理
	void Initialize(ID3D11Device* device);

	// Gui描画
	void DrawGui() override;

	// マテリアルの編集GUI表示
	void DrawMaterialEditGui(Material* material, ModelRenderType type, bool writeGBuffer);

	// インスタンシングモデルのマテリアルの編集GUI表示
	void DrawInstancingMaterialEditGui(Material* material);

	// シェーダーマップ取得
	ShaderMap* GetShaderMap(bool writeGBuffer)
	{
		return writeGBuffer ? _deferredShaders : _forwardShaders;
	}
	// カスケードシャドウマップ用シェーダー取得
	ModelShaderBase* GetCascadedSMShader(ModelRenderType type)
	{
		return _cascadedSMShader[static_cast<int>(type)].get();
	}
	// 使用可能なシェーダー名を取得
	std::vector<const char*> GetShaderNames(ModelRenderType type, bool writeGBuffer)
	{
		std::vector<const char*> shaderTypes;
		ShaderMap& shaderMap = writeGBuffer ? 
			_deferredShaders[static_cast<int>(type)] :
			_forwardShaders[static_cast<int>(type)];
		for (auto& [name, shader] : shaderMap)
		{
			// shaderがnullptrの時はスキップ
			if (shader == nullptr)
				continue;
			shaderTypes.push_back(name.c_str());
		}
		return shaderTypes;
	}
	// インスタンシング描画で使用可能なシェーダーを取得
	std::vector<const char*> GetInstancingShaderNames()
	{
		std::vector<const char*> shaderTypes;
		auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
		for (auto& [name, shader] : modelShaderResource->GetShaderMap(false)[static_cast<int>(ModelRenderType::Instancing)])
		{
			// shaderがnullptrの時はスキップ
			if (shader == nullptr)
				continue;
			shaderTypes.push_back(name.c_str());
		}
		return shaderTypes;
	}
	// typeとkeyからパラメータのkeyを取得
	Material::ParameterMap GetShaderParameterKey(ModelRenderType type, std::string key, bool deferred);

private:
	const std::string _name = "ModelShader";
	const std::string _filePath = "";

	ShaderMap                   _deferredShaders[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];
	ShaderMap                   _forwardShaders[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];
	std::unique_ptr<ModelShaderBase> _cascadedSMShader[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];
};

// リソース設定
_REGISTER_RESOURCE(ModelShaderResource)
