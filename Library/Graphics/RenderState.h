#pragma once

#include <wrl.h>
#include <d3d11.h>

// サンプラステート
enum class SamplerState
{
	PointWrap,
	PointClamp,
	LinearWrap,
	LinearClamp,
	BorderPoint,	// カスケードシャドウマップ
	Comparison,		// カスケードシャドウマップ

	EnumCount
};

// デプスステート
enum class DepthState
{
	TestAndWrite,
	TestOnly,
	WriteOnly,
	NoTestNoWrite,

	EnumCount
};

// ブレンドステート
enum class BlendState
{
	Opaque,// 合成なし
	Transparency,
	Additive,
	Subtraction,
	Multiply,
	None,

	Alpha,

	MultipleRenderTargets,	// マルチレンダーターゲット用

	EnumCount
};

// ラスタライザステート
enum class RasterizerState
{
	SolidCullNone,
	SolidCullBack,
	WireCullNone,
	WireCullBack,

	EnumCount
};

// 各種ステートの管理者
class RenderState
{
public:
	RenderState(ID3D11Device* device);
	~RenderState() = default;

	// サンプラステート取得
	ID3D11SamplerState* GetSamplerState(SamplerState state) const
	{
		return samplerStates_[static_cast<int>(state)].Get();
	}
	ID3D11SamplerState*const* GetAddressOfSamplerState(SamplerState state) const
	{
		return samplerStates_[static_cast<int>(state)].GetAddressOf();
	}

	// デプスステート取得
	ID3D11DepthStencilState* GetDepthStencilState(DepthState state) const
	{
		return depthStencilStates_[static_cast<int>(state)].Get();
	}

	// ブレンドステート取得
	ID3D11BlendState* GetBlendState(BlendState state) const
	{
		return blendStates_[static_cast<int>(state)].Get();
	}
	ID3D11BlendState*const* GetAddressOfBlendState(BlendState state) const
	{
		return blendStates_[static_cast<int>(state)].GetAddressOf();
	}

	// ラスタライザーステート取得
	ID3D11RasterizerState* GetRasterizerState(RasterizerState state) const
	{
		return rasterizerStates_[static_cast<int>(state)].Get();
	}

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerStates_[static_cast<int>(SamplerState::EnumCount)];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates_[static_cast<int>(DepthState::EnumCount)];
	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendStates_[static_cast<int>(BlendState::EnumCount)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerStates_[static_cast<int>(RasterizerState::EnumCount)];
};