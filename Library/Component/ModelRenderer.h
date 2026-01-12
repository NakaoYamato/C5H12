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
	// カメラからの距離取得
    float GetCameraDistance() const { return _cameraDistance; }
	// カメラ距離による透明化が有効か
	bool IsCameraDistanceAlphaEnabled() const { return _cameraDistanceAlphaEnabled; }
	// カメラ距離によるアルファ値補正
	float GetCameraDistanceAlpha() const { return _cameraDistanceAlpha; }
	// カメラ距離によるアルファ値補正最小値
	float GetCameraDistanceAlphaMin() const { return _cameraDistanceAlphaMin; }
	// カメラ距離によるアルファ値補正開始距離(m)
	float GetCameraDistanceAlphaStart() const { return _cameraDistanceAlphaStart; }

	// カメラ距離による透明化設定
	void SetCameraDistanceAlphaEnabled(bool enabled) { _cameraDistanceAlphaEnabled = enabled; }
	// カメラ距離によるアルファ値補正最小値設定
	void SetCameraDistanceAlphaMin(float min) { _cameraDistanceAlphaMin = min; }
	// カメラ距離によるアルファ値補正開始距離(m)設定
	void SetCameraDistanceAlphaStart(float start) { _cameraDistanceAlphaStart = start; }

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

#pragma region LOD
    // 中LODメッシュ適応距離(m)取得
    float GetMidLodDistance() const { return _midLodDistance; }
    // 低LODメッシュ適応距離(m)取得
    float GetLowLodDistance() const { return _lowLodDistance; }
    // 中LODメッシュ適応距離(m)設定
    void SetMidLodDistance(float distance) { _midLodDistance = distance; }
    // 低LODメッシュ適応距離(m)設定
    void SetLowLodDistance(float distance) { _lowLodDistance = distance; }
    // 中LODメッシュ使用中か取得
    bool IsMidLod() const { return _isMidLod; }
    // 低LODメッシュ使用中か取得
    bool IsLowLod() const { return _isLowLod; }
#pragma endregion

private:
	std::weak_ptr<Model> _model;
	// モデルのボーン行列
	std::unordered_map<int, std::vector<DirectX::XMFLOAT4X4>> _boneTransformMap;

	ModelRenderType _renderType = ModelRenderType::Dynamic;
	Vector4 _color{ 1,1,1,1 };

	// カメラからの距離
    float _cameraDistance = 0.0f;

	// カメラ距離による透明化を行うか
	bool _cameraDistanceAlphaEnabled = true;
	// カメラ距離によるアルファ値補正
	float _cameraDistanceAlpha = 1.0f;
	// カメラ距離によるアルファ値補正最小値
	float _cameraDistanceAlphaMin = 0.3f;
	// カメラ距離によるアルファ値補正開始距離(m)
	float _cameraDistanceAlphaStart = 3.0f;

	// マテリアル
	std::vector<Material> _materialMap;

	// メッシュの描画非表示リスト
	std::unordered_map<std::string, bool> _hiddenMeshMap;

	// 中LODメッシュ適応距離(m)
    float _midLodDistance = 25.0f;
    // 低LODメッシュ適応距離(m)
    float _lowLodDistance = 50.0f;

    // 中LODメッシュ使用中か
    bool _isMidLod = false;
    // 低LODメッシュ使用中か
    bool _isLowLod = false;
};