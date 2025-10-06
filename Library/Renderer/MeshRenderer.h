#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "../Model/Model.h"
#include "../Material/Material.h"
#include "../Shader/Model/ModelShaderBase.h"

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

class MeshRenderer
{
private:
    static const int FBX_MAX_BONES = 256;
    static const int INSTANCED_MAX = 256;

#pragma region 定数バッファ
    // 通常モデル用
    struct DynamicBoneCB
    {
        DirectX::XMFLOAT4   materialColor{ 1,1,1,1 };
        DirectX::XMFLOAT4X4 boneTransforms[FBX_MAX_BONES]
    {
        {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        }
    };
    };
    // アニメーションなしモデル用
    struct StaticBoneCB
    {
        DirectX::XMFLOAT4   materialColor{ 1,1,1,1 };
        DirectX::XMFLOAT4X4 world{};
    };
#pragma endregion

#pragma region Info
	// 描画用情報
    struct DrawInfo
    {
        Model*                      model       = nullptr;
        const ModelResource::Mesh*  mesh        = nullptr;
        Vector4				        color       = Vector4::White;
        Material*                   material    = nullptr;
    };
	// 半透明描画用情報
    struct AlphaDrawInfo
    {
		DrawInfo				    drawInfo;
        ModelRenderType             renderType;
        float			            distance = 0.0f;
    };
    // インスタンシング描画用情報
    struct InstancingDrawInfo
    {
        struct Data
        {
			DirectX::XMFLOAT4X4     world{};
			Vector4                 color = Vector4::White;
        };

        Model*                      model = nullptr;
		std::vector<Data>           modelDatas;
        Material*                   material = nullptr;
    };
#pragma endregion
public:
	MeshRenderer() = default;
	~MeshRenderer() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device"></param>
    void Initialize(ID3D11Device* device);

    /// <summary>
    /// メッシュ描画
    /// </summary>
    /// <param name="mesh"></param>
    /// <param name="model"></param>
    /// <param name="color"></param>
    /// <param name="material"></param>
    /// <param name="renderType"></param>
    /// <param name="parameter"></param>
    void Draw(const ModelResource::Mesh* mesh,
        Model* model,
        const Vector4& color,
        Material* material,
        ModelRenderType renderType);

	/// <summary>
	/// メッシュのテスト描画
	/// </summary>
	/// <param name="mesh"></param>
	/// <param name="model"></param>
	/// <param name="renderType"></param>
	void DrawTest(const ModelResource::Mesh* mesh, Model* model, ModelRenderType renderType);
	/// <summary>
	/// メッシュのテスト描画
	/// </summary>
	/// <param name="model"></param>
	/// <param name="world"></param>
	void DrawTest(Model* model, const DirectX::XMFLOAT4X4& world);

	/// <summary>
	/// 影描画
	/// </summary>
	/// <param name="mesh"></param>
	/// <param name="model"></param>
	/// <param name="color"></param>
	/// <param name="material"></param>
	/// <param name="renderType"></param>
	/// <param name="parameter"></param>
	void DrawShadow(const ModelResource::Mesh* mesh,
        Model* model,
        const Vector4& color,
        Material* material,
        ModelRenderType renderType);

    /// <summary>
    /// インスタンシングモデルの描画
    /// </summary>
    /// <param name="model"></param>
    /// <param name="color"></param>
    /// <param name="material"></param>
    /// <param name="shaderType"></param>
    /// <param name="world"></param>
    void DrawInstancing(Model* model,
        const Vector4& color,
        Material* material,
        const DirectX::XMFLOAT4X4& world);

    /// <summary>
    /// 描画実行
    /// シーン用定数バッファはこの関数を呼ぶ前に行う
    /// </summary>
    /// <param name="rc"></param>
    void RenderOpaque(const RenderContext& rc, bool writeGBuffer);

    /// <summary>
    /// 半透明描画
    /// </summary>
    /// <param name="rc"></param>
    void RenderAlpha(const RenderContext& rc);

    /// <summary>
    /// 影描画実行
    /// </summary>
    /// <param name="rc"></param>
    void CastShadow(const RenderContext& rc);

    /// <summary>
    /// デファードレンダリングで使用可能なシェーダーを取得
    /// </summary>
    /// <param name="type"></param>
    /// <param name="deferred"></param>
    /// <returns></returns>
    std::vector<const char*> GetDeferredShaderNames(ModelRenderType type);

    /// <summary>
    /// フォワードレンダリングで使用可能なシェーダーを取得
    /// </summary>
    /// <param name="type"></param>
    /// <param name="deferred"></param>
    /// <returns></returns>
    std::vector<const char*> GetForwardShaderNames(ModelRenderType type);

    /// <summary>
	/// インスタンシング描画で使用可能なシェーダーを取得
    /// </summary>
    /// <param name="type"></param>
    /// <returns></returns>
    std::vector<const char*> GetInstancingShaderNames();

    /// <summary>
    /// typeとkeyからパラメータのkeyを取得
    /// </summary>
    /// <param name="type"></param>
    /// <param name="key"></param>
    /// <returns></returns>
    Material::ParameterMap GetShaderParameterKey(ModelRenderType type, std::string key, bool deferred);

private:
    // インスタンシングモデルの描画
    void RenderInstancing(const RenderContext& rc);

    // DynamicBoneModelのメッシュ描画
    void DrawDynamicBoneMesh(const RenderContext& rc, ModelShaderBase* shader, DrawInfo& drawInfo);

    // StaticBoneModelのメッシュ描画
    void DrawStaticBoneModel(const RenderContext& rc, ModelShaderBase* shader, DrawInfo& drawInfo);

private:
    // 定数バッファのデータ
    // サイズが大きいく関数内で定義するとスタック警告がでるため静的に確保
    DynamicBoneCB		_cbDynamicSkeleton{};
    StaticBoneCB		_cbStaticSkeleton{};

    // シェーダーの配列
    using ShaderMap = std::unordered_map<std::string, std::unique_ptr<ModelShaderBase>>;
    ShaderMap                   _deferredShaders[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];
    ShaderMap                   _forwardShaders[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];
    std::unique_ptr<ModelShaderBase> _cascadedSMShader[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];

    // 各モデルタイプのInfo
	// Key : シェーダー名
    using DrawInfoMap = std::unordered_map<std::string, std::vector<DrawInfo>>;
    DrawInfoMap                 _dynamicInfomap;
    DrawInfoMap                 _staticInfomap;
    std::vector<AlphaDrawInfo>	_alphaDrawInfomap;
    // Key : シェーダー名+モデルファイル名
    using InstancingDrawInfoMap = std::unordered_map<std::string, InstancingDrawInfo>;
    InstancingDrawInfoMap       _instancingInfoMap;
    Microsoft::WRL::ComPtr<ID3D11Buffer>				_instancingDataBuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	_instancingDataSRV;

    // 各定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>	_dynamicBoneCB;
    Microsoft::WRL::ComPtr<ID3D11Buffer>	_staticBoneCB;

    // テスト描画用マテリアル
    Material _testMaterial{};
};
