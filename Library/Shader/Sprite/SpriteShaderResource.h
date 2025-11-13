#pragma once

#include "SpriteShaderBase.h"
#include "../ShaderResourceBase.h"

class SpriteShaderResource : public ShaderResourceBase
{
public:
	SpriteShaderResource() = default;
	~SpriteShaderResource() override {}

	// 名前取得
	std::string GetName() const override { return _name; }
	// ファイルパス取得
	std::string GetFilePath() const override { return _filePath; }
	// ファイル読み込み
	bool LoadFromFile() override { return false; }
	// ファイル保存
	bool SaveToFile() override { return false; }
	// シェーダータイプ取得
	ShaderType GetShaderType() const override { return ShaderType::Sprite; }

	// 初期化処理
	void Initialize(ID3D11Device* device);
	// Gui描画
	void DrawGui() override;

	// マテリアルの編集GUI表示
	void DrawMaterialEditGui(Material* material);

	// シェーダー取得
	SpriteShaderBase* GetShader(const std::string& name)
	{
		if (_shaders.find(name) != _shaders.end())
		{
			return _shaders.at(name).get();
		}
		return nullptr;
	}

	// シェーダー名一覧取得
	std::vector<const char*> GetShaderNames() const
	{
		std::vector<const char*> names;
		for (const auto& [name, shader] : _shaders)
		{
			names.push_back(name.c_str());
		}
		return names;
	}

	// パラメータkey取得
	Material::ParameterMap GetShaderParameterKey(const std::string& name)
	{
		std::vector<std::string> keys;
		if (_shaders.find(name) != _shaders.end())
		{
			return _shaders.at(name)->GetParameterMap();
		}
		return Material::ParameterMap();
	}

	// Blit頂点シェーダー取得
	ID3D11VertexShader* GetBlitVS() { return _blitVertexShader.Get(); }
	// Blitピクセルシェーダー取得
	ID3D11PixelShader* GetBlitPS() { return _blitPixelShader.Get(); }

private:
	const std::string _name = "SpriteShader";
	const std::string _filePath = "";

	std::unordered_map<std::string, std::unique_ptr<SpriteShaderBase>> _shaders{};

	VertexShader _blitVertexShader;
	PixelShader	_blitPixelShader;
};

// リソース設定
_REGISTER_RESOURCE(SpriteShaderResource)
