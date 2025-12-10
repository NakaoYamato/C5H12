#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <memory>
#include <vector>
#include <functional>

#include "../../Graphics/RenderContext.h"
#include "../../Library/Math/Vector.h"
#include "../../2D/SpriteResource.h"

// カスケードシャドウマップ用定数バッファスロット
#define _CASCADED_SHADOW_MAP_CB_SLOT_INDEX 3

class CascadedShadowMap
{
public:
    // モデルの影を描画するときに必要な定数バッファ : スロット3
    struct Constants
    {
        DirectX::XMFLOAT4X4 cascadedMatrices[4]{};
        float cascadedPlaneDistances[4]{};
    };
    // カスケードシャドウマップのパラメーター定数バッファ : スロット2
    struct ParametricConstants
    {
        float shadowColor{ 0.45f };
        float shadowDepthBias{ 0.0001f };
        float colorizeCascadedLayer{ 0.0f };
        float padding;
    };

    CascadedShadowMap(ID3D11Device* device, UINT width, UINT height, UINT cascadeCount = 4);
    ~CascadedShadowMap() {}
public:
    // 更新処理
	void Update(float elapsedTime);
    // 影の生成開始
    void Activate(const RenderContext& rc, const UINT& cbSlot);
    // 深度バッファのクリア
    void Clear(ID3D11DeviceContext* immediateContext);
	// 影の生成開始（クリアしてから）
    void ClearAndActivate(const RenderContext& rc);
    // 影の生成終了
    void Deactivate(const RenderContext& rc);
    // ImGui描画
    void DrawGui();
    // カスケードシャドウマップ描画のための定数バッファ更新
    void UpdateCSMConstants(const RenderContext& rc);
    // 影の描画
    void Blit(ID3D11DeviceContext* immediateContext,
        ID3D11ShaderResourceView* colorSRV,
        ID3D11ShaderResourceView* depthSRV,
        ID3D11ShaderResourceView* normalSRV);
    // 深度値のSRV取得
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthMap()
    {
        return _shaderResourceView;
    }
	bool IsCreateShadow() const{ return _createShadow; }
private:
    // カスケード数
    const UINT _cascadeCount;
    // 分割方式の重み付け
    float _splitSchemeWeight = 0.95f;
    // カスケードへのフィット
    bool _fitToCascade = true;
    // Z方向の拡大率
    float _zMult = 10.0f;
    // 臨界深度値
    float _criticalDepthValue = 0.0f;

    // 影の生成タイマー
    float _createShadowTimer = 0.0f;
    // 影の更新間隔
    float _createShadowInterval = 1.0f / 60.0f;
    // 影を生成するかどうかのフラグ
    bool _createShadow = true;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> _depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depthStencilView;
    D3D11_VIEWPORT _viewport{};

    // カスケードごとのビュー・プロジェクション行列と分割平面距離
    std::vector<DirectX::XMFLOAT4X4> _cascadedMatrices;
    std::vector<float> _cascadedPlaneDistances;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
    // デバッグ表示用に、カスケードごとのSRVを保持する配列
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> _debugSRVs;

    Microsoft::WRL::ComPtr<ID3D11Buffer> _constantBuffer;

    Microsoft::WRL::ComPtr<ID3D11Buffer> _parametricConstantBuffer;
    ParametricConstants _parametricConstants{};

    // キャッシュ用
    UINT _viewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
    D3D11_VIEWPORT _cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _cachedRTV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _cachedDSV;

    std::unique_ptr<SpriteResource> _fullscreenQuad;

};