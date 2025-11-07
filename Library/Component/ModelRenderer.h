#pragma once

#include "Component.h"
#include "../../Library/Renderer/MeshRenderer.h"

/// <summary>
/// モデルコンポーネント
/// </summary>
class ModelRenderer : public Component
{
public:
	ModelRenderer() {}
	~ModelRenderer()override {}

	// 名前取得
	const char* GetName()const { return "ModelRenderer"; }

	// 生成時処理
	void OnCreate()override;
	// 遅延更新処理
	void LateUpdate(float elapsedTime) override;
	// 描画処理
	void Render(const RenderContext& rc) override;
	// 影描画
	void CastShadow(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

	// 指定のマテリアルのSRVを変更
	void ChangeMaterialSRV(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv,
		int materialIndex,
		std::string textureKey);
	// 指定のマテリアルのSRVを変更
	void ChangeMaterialSRV(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv,
		std::string materialName,
		std::string textureKey);

#pragma region アクセサ
	ModelRenderType GetRenderType()const { return _renderType; }
	const Vector4& GetColor()const { return _color; }
	std::vector<Material>& GetMaterials() { return _materialMap; }
	Material& GetMaterial(int index) { return _materialMap[index]; }
	Material& GetMaterial(const std::string& name);
	bool IsMeshHidden(const std::string& meshName) const {
		auto it = _hiddenMeshMap.find(meshName);
		if (it != _hiddenMeshMap.end()) {
			return it->second;
		}
		return false;
	}
	// メッシュの描画非表示リスト取得
	const std::unordered_map<std::string, bool>& GetHiddenMeshMap() const { return _hiddenMeshMap; }

	// モデル設定
	void SetModel(std::weak_ptr<Model> model);
	void SetRenderType(ModelRenderType type) { this->_renderType = type; }
	void SetColor(const Vector4& c) { this->_color = c; }
	void SetMeshHidden(const std::string& meshName, bool hidden)
	{
		_hiddenMeshMap[meshName] = hidden;
	}
#pragma endregion
private:
	std::weak_ptr<Model> _model;

	ModelRenderType _renderType = ModelRenderType::Dynamic;
	Vector4 _color{ 1,1,1,1 };

	// マテリアル
	std::vector<Material> _materialMap;

	// メッシュの描画非表示リスト
	std::unordered_map<std::string, bool> _hiddenMeshMap;
};