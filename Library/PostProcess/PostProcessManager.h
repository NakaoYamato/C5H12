#pragma once

#include <d3d11.h>
#include <memory>
#include <map>
#include <string>

#include "PostProcessBase.h"
#include "../Graphics/RenderContext.h"

/// <summary>
/// ポストプロセスの種類
/// </summary>
enum class PostProcessType
{
	GaussianFilterPP,
	RadialBlurPP,
	ChromaticAberrationPP,
	BloomPP,
	RobertsCrossPP,

	FinalPassPP,

	MAX_PostProcessType
};

/// <summary>
/// ポストプロセスの管理者
/// </summary>
class PostProcessManager
{
private:
	PostProcessManager() {}
	~PostProcessManager() {}

public:
	static PostProcessManager& Instance() {
		static PostProcessManager ins;
		return ins;
	}

	// 初期化処理
	void Initialize(ID3D11Device* device, uint32_t width, uint32_t height);

	// 更新処理
	void Update(float elapsedTime);

	/// <summary>
	/// ポストプロセスをかける
	/// </summary>
	/// <param name="rc"></param>
	/// <param name="srcSRV"></param>
	void ApplyEffect(RenderContext& rc,
		ID3D11ShaderResourceView** srcSRV);

	// Gui描画
	void DrawGui();

	// アクセサ
	[[nodiscard]] PostProcessBase* GetPostProcess(PostProcessType type) {
		return _postProcesses[static_cast<int>(type)].first.get();
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetAppliedEffectSRV() {
		return _appliedEffectSRV;
	}

private:
	using GuiFlag = std::pair<std::string, bool>;
	// ポストプロセスの配列
	// bool	: GUIの使用フラグ
	std::pair<std::unique_ptr<PostProcessBase>, GuiFlag> _postProcesses[static_cast<int>(PostProcessType::MAX_PostProcessType)];

	// ポストプロセスをかけた後のSRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _appliedEffectSRV;

	// ブルーム用
	std::unique_ptr<FrameBuffer> _bloomRenderFrame;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _bloomPS;
	std::unique_ptr<Sprite> _fullscreenQuad;

};