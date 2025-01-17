#include "PostProcessManager.h"

#include<imgui.h>

#include "../Converter/ToString.h"

#include "../PostProcess/ColorFilter/ColorFilter.h"
#include "../PostProcess/Bloom/Bloom.h"
#include "../PostProcess/GaussianFilter/GaussianFilter.h"
#include "../PostProcess/Tonemapping/Tonemapping.h"
#include "../PostProcess/RadialBlur/RadialBlur.h"
#include "../PostProcess/Vignette/Vignette.h"
#include "../PostProcess/ChromaticAberration/ChromaticAberration.h"

void PostProcessManager::Initialize(ID3D11Device* device, uint32_t width, uint32_t height)
{
	{
		// カラーフィルター
		postProcesses[static_cast<int>(PostProcessType::ColorFilterPP)].first =
			std::make_unique<ColorFilter>(device,
				width, height);
		auto& [name, flag] = postProcesses[static_cast<int>(PostProcessType::ColorFilterPP)].second;
		name = TO_STRING_U8(ColorFilter);
	}
	{
		// ガウスブラー(ぼかし)
		postProcesses[static_cast<int>(PostProcessType::GaussianFilterPP)].first =
			std::make_unique<GaussianFilter>(device,
				width, height);
		auto& [name, flag] = postProcesses[static_cast<int>(PostProcessType::GaussianFilterPP)].second;
		name = TO_STRING_U8(GaussianFilter);
	}
	{
		// 河瀬式ブルーム
		postProcesses[static_cast<int>(PostProcessType::BloomPP)].first =
			std::make_unique<Bloom>(device,
				width, height);
		auto& [name, flag] = postProcesses[static_cast<int>(PostProcessType::BloomPP)].second;
		name = TO_STRING_U8(Bloom);
	}
	{
		// トーンマッピング
		postProcesses[static_cast<int>(PostProcessType::TonemappingPP)].first =
			std::make_unique<Tonemapping>(device,
				width, height);
		auto& [name, flag] = postProcesses[static_cast<int>(PostProcessType::TonemappingPP)].second;
		name = TO_STRING_U8(Tonemapping);
	}
	{
		// ラジアルブラー
		postProcesses[static_cast<int>(PostProcessType::RadialBlurPP)].first =
			std::make_unique<RadialBlur>(device,
				width, height);
		auto& [name, flag] = postProcesses[static_cast<int>(PostProcessType::RadialBlurPP)].second;
		name = TO_STRING_U8(RadialBlur);
	}
	{
		// ヴィネット
		postProcesses[static_cast<int>(PostProcessType::VignettePP)].first =
			std::make_unique<Vignette>(device,
				width, height);
		auto& [name, flag] = postProcesses[static_cast<int>(PostProcessType::VignettePP)].second;
		name = TO_STRING_U8(Vignette);
	}
	{
		// 色収差
		postProcesses[static_cast<int>(PostProcessType::ChromaticAberrationPP)].first =
			std::make_unique<ChromaticAberration>(device,
				width, height);
		auto& [name, flag] = postProcesses[static_cast<int>(PostProcessType::ChromaticAberrationPP)].second;
		name = TO_STRING_U8(ChromaticAberration);
	}
}

void PostProcessManager::Update(float elapsedTime)
{
	for (size_t i = 0; i < static_cast<int>(PostProcessType::MAX_PostProcessType); ++i)
	{
		// 要素チェック
		if (postProcesses[i].first == nullptr)continue;

		// 更新処理
		postProcesses[i].first->Update(elapsedTime);
	}
}

void PostProcessManager::DrawGui()
{
	// メニューバー
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"ポストエフェクトGUI"))
		{
			for (size_t i = 0; i < static_cast<int>(PostProcessType::MAX_PostProcessType); ++i)
			{
				GuiFlag& guiFlag = postProcesses[i].second;
				ImGui::Checkbox(guiFlag.first.c_str(), &guiFlag.second);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	for (size_t i = 0; i < static_cast<int>(PostProcessType::MAX_PostProcessType); ++i)
	{
		GuiFlag& guiFlag = postProcesses[i].second;
		if (guiFlag.second == false)
			continue;

		// 要素チェック
		if (postProcesses[i].first == nullptr)continue;

		// Gui描画
		postProcesses[i].first->DrawGui();
	}
}
