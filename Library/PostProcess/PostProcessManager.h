#pragma once

#include <d3d11.h>
#include <memory>
#include <map>
#include <string>

#include "PostProcessBase.h"

/// <summary>
/// ポストプロセスの種類
/// </summary>
enum class PostProcessType
{
	ColorFilterPP,
	GaussianFilterPP,
	TonemappingPP,
	RadialBlurPP,
	VignettePP,
	ChromaticAberrationPP,
	BloomPP,

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

	// Gui描画
	void DrawGui();

	// アクセサ
	[[nodiscard]] PostProcessBase* GetPostProcess(PostProcessType type) {
		return postProcesses[static_cast<int>(type)].first.get();
	}

private:
	using GuiFlag = std::pair<std::string, bool>;
	// ポストエフェクトの配列
	// bool	: GUIの使用フラグ
	std::pair<std::unique_ptr<PostProcessBase>, GuiFlag> postProcesses[static_cast<int>(PostProcessType::MAX_PostProcessType)];
};