#pragma once

#include <string>
#include <unordered_map>
#include <wrl.h>
#include <d3d11.h>
#include <variant>

#include "../../Library/Math/Vector.h"
#include "../../Library/Graphics/Texture.h"
#include "../../Library/Exporter/Exporter.h"

/// <summary>
/// ブレンドタイプ
/// </summary>
enum class BlendType
{
	Opaque,
	Alpha,

	BlendTypeMax
};

enum class ShaderType
{
	Model,
	Primitive,
	Sprite,

	ShaderTypeMax
};

class Material
{
public:
	using VariantType = std::variant<int, float, Vector2, Vector3, Vector4>;
	using ColorMap = std::unordered_map<std::string, Vector4>;
	using TextureMap = std::unordered_map<std::string, Texture>;
	using ParameterMap = std::unordered_map<std::string, VariantType>;
public:
	Material() = default;
	Material(const std::string& name) : _name(name) {}
	~Material() = default;

	/// <summary>
	/// デバッグGUI表示
	/// </summary>
	void DrawGui();

	/// <summary>
	/// ファイルからテクスチャを読み込む
	/// </summary>
	/// <param name="key"></param>
	/// <param name="filename"></param>
	void LoadTexture(const std::string& key,
		const wchar_t* filename);

	/// <summary>
	/// ダミーテクスチャ作成
	/// </summary>
	/// <param name="key"></param>
	/// <param name="value"></param>
	/// <param name="dimension"></param>
	void MakeDummyTexture(const std::string& key,
		DWORD value,
		UINT dimension);
	
	/// <summary>
	/// 指定のマテリアルのSRVを変更
	/// </summary>
	/// <param name="srv"></param>
	/// <param name="key"></param>
	void ChangeTextureSRV(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv,
		const std::string& key)
	{
		_textureDatas[key].Set(srv);
	}
#pragma region アクセサ
	// マテリアル名取得
	const std::string&								GetName() const { return _name; }
	// シェーダータイプ取得
	ShaderType										GetShaderType() const { return _shaderType; }
	// 色取得
	const Vector4&									GetColor(const std::string& key) const { return _colors.at(key); }
	// テクスチャ情報取得
	const Texture&									GetTextureData(const std::string& key) const { return _textureDatas.at(key); }
	// パラメータ取得
	const VariantType&								GetParameter(const std::string& key) const { return _parameters.at(key); }
	// パラメータ(int1)取得　失敗でnullptr
	const int*										GetParameterI1(const std::string& key) const { 
		if (_parameters.find(key) == _parameters.end()) return nullptr;
		return std::get_if<int>(&_parameters.at(key)); 
	}
	// パラメータ(float1)取得　失敗でnullptr
	const float*									GetParameterF1(const std::string& key) const {
		if (_parameters.find(key) == _parameters.end()) return nullptr;
		return std::get_if<float>(&_parameters.at(key)); 
	}
	// パラメータ(Vector2)取得　失敗でnullptr
	const Vector2*									GetParameterF2(const std::string& key) const {
		if (_parameters.find(key) == _parameters.end()) return nullptr;
		return std::get_if<Vector2>(&_parameters.at(key)); 
	}
	// パラメータ(Vector3)取得　失敗でnullptr
	const Vector3*									GetParameterF3(const std::string& key) const {
		if (_parameters.find(key) == _parameters.end()) return nullptr; 
		return std::get_if<Vector3>(&_parameters.at(key)); 
	}
	// パラメータ(Vector4)取得　失敗でnullptr
	const Vector4*									GetParameterF4(const std::string& key) const {
		if (_parameters.find(key) == _parameters.end()) return nullptr; 
		return std::get_if<Vector4>(&_parameters.at(key)); 
	}
	// ブレンドタイプ取得
	BlendType										GetBlendType() const { return _blendType; }
	// シェーダー名取得
	const std::string&								GetShaderName() const { return _shaderName; }

	// マテリアル名設定
	void SetName(const std::string& name) { _name = name; }
	// シェーダータイプ設定
	void SetShaderType(ShaderType type) { _shaderType = type; }
	// 色設定
	void SetColor(const std::string& key, const Vector4& color) { _colors[key] = color; }
	// テクスチャ設定
	void SetTextureData(const std::string& key, const Texture& textureData) { _textureDatas[key] = textureData; }
	// パラメータ設定
	void SetParameter(const std::string& key, const VariantType& parameter) { _parameters[key] = parameter; }
	// パラメータ設定
	void SetParameter(const std::string& key, int parameter) { _parameters[key] = parameter; }
	// パラメータ設定
	void SetParameter(const std::string& key, float parameter) { _parameters[key] = parameter; }
	// パラメータ設定
	void SetParameter(const std::string& key, const Vector2& parameter) { _parameters[key] = parameter; }
	// パラメータ設定
	void SetParameter(const std::string& key, const Vector3& parameter) { _parameters[key] = parameter; }
	// パラメータ設定
	void SetParameter(const std::string& key, const Vector4& parameter) { _parameters[key] = parameter; }
	// パラメータ設定
	void SetParameterMap(const Material::ParameterMap& parameter) { _parameters = parameter; }
	// ブレンドタイプ設定
	void SetBlendType(BlendType type) { _blendType = type; }
	// シェーダー名設定
	void SetShaderName(std::string type) { _shaderName = type; }
#pragma endregion

#pragma region 入出力
	// ファイル読み込み
	bool LoadFromFile(nlohmann::json_abi_v3_12_0::json& json);
	// ファイル保存
	bool SaveToFile(nlohmann::json_abi_v3_12_0::json& json);
#pragma endregion

private:
	// ColorMapGUI描画
	void DrawColorMapGui();
	// TextureMapGUI描画
	void DrawTextureMapGui();
	// ParameterMapGUI描画
	void DrawParameterMapGui();

private:
	std::string		_name;
	ShaderType		_shaderType = ShaderType::Model;
	ColorMap		_colors;
	TextureMap		_textureDatas;
	ParameterMap	_parameters;

	BlendType		_blendType = BlendType::Opaque;
	std::string		_shaderName = "";
};