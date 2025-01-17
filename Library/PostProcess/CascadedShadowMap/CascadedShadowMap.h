#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <vector>
#include <functional>

#include "../../Graphics/RenderContext.h"
#include "../../Library/Math/Vector.h"

// 福井先生のひみつフォルダから引用
// 参考資料 https://learnopengl.com/Guest-Articles/2021/CSM
//         https://learn.microsoft.com/en-us/windows/win32/dxtecharts/cascaded-shadow-maps
//         https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-10-parallel-split-shadow-maps-programmable-gpus

class CascadedShadowMap
{
public:
    CascadedShadowMap(ID3D11Device* device, UINT width, UINT height, UINT cascadeCount = 4);
    virtual ~CascadedShadowMap() = default;

    CascadedShadowMap() = delete;
    CascadedShadowMap(const CascadedShadowMap&) = delete;
    CascadedShadowMap(const CascadedShadowMap&&) noexcept = delete;
    CascadedShadowMap& operator =(CascadedShadowMap&) = delete;
    CascadedShadowMap& operator =(CascadedShadowMap&&) noexcept = delete;

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    D3D11_VIEWPORT viewport{};

    std::vector<DirectX::XMFLOAT4X4> cascadedMatrices;
    std::vector<float> cascadedPlaneDistances;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

    // モデルの影を描画するときに必要な定数バッファ : スロット3
    struct Constants
    {
        DirectX::XMFLOAT4X4 cascadedMatrices[4]{};
        float cascadedPlaneDistances[4]{};
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

    // カスケードシャドウマップのパラメーター定数バッファ : スロット2
    struct ParametricConstants
    {
        float shadowColor{ 0.2f };
        float shadowDepthBias{ 0.0001f };
        float colorizeCascadedLayer{ 0.0f };
        float pad{};
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> parametricConstantBuffer;

public:
    // 影の生成開始
    void Activate(const RenderContext& rc,
        const UINT& cbSlot);
    void ClearAndActive(const RenderContext& rc,
        const UINT& cbSlot);

    // 影の生成終了
    void Deactivate(const RenderContext& rc);

    // ImGui描画
    void DrawGui();

    void Clear(ID3D11DeviceContext* immediateContext)
    {
        immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
    }

    // カスケードシャドウマップ描画のための定数バッファ更新
    void UpdateCSMConstants(const RenderContext& rc);

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthMap()
    {
        return shaderResourceView;
    }

public:
    const UINT cascadeCount;
    float splitSchemeWeight = 0.7f;// logarithmic_split_scheme * _split_scheme_weight + uniform_split_scheme * (1 - _split_scheme_weight)
    // https://learn.microsoft.com/en-us/windows/win32/dxtecharts/cascaded-shadow-maps
    // Fit to scene vs.fit to cascade
    // - Fit to Scene
    //	All of the frusta can be created with the same near plane.This forces the cascades to overlap.
    // - Fit to Cascade
    //	Alternatively, frusta can be created with the actual partition interval being used as near and far planes.This causes a tighter fit, but degenerates to fit to scene in the case of dueling frusta.
    // Fit to cascade wastes less resolution.The problem with fit to cascade is that the orthographic projection grows and shrinks based on the orientation of the view frustum.
    // The fit to scene technique pads the orthographic projection by the max size of the view frustum removing the artifacts that appear when the view - camera moves.
        // Common Techniques to Improve Shadow Depth Maps addresses the artifacts that appear when the light moves in the section "Moving the light in texel sized increments."
    bool fitToCascade = true;
    // Before creating the actual projection matrix we are going to increase the size of the space covered by the nearand far plane of the light frustum.
    // We do this by "pulling back" the near plane, and "pushing away" the far plane.In the code we achieve this by dividing or multiplying by zMult.
    // This is because we want to include geometry which is behind or in front of our frustum in camera space. Think about it : not only geometry which 
    // is in the frustum can cast shadows on a surface in the frustum!
    float zMult = 10.0f;
    float criticalDepthValue = 0.0f;

private:
    UINT viewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
    D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;

    ParametricConstants parametricConstants{};
};