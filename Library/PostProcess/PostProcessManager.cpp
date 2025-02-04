#include "PostProcessManager.h"

#include<imgui.h>

#include "../Converter/ToString.h"
#include "../ResourceManager/GpuResourceManager.h"

#include "../PostProcess/ColorFilter/ColorFilter.h"
#include "../PostProcess/Bloom/Bloom.h"
#include "../PostProcess/GaussianFilter/GaussianFilter.h"
#include "../PostProcess/Tonemapping/Tonemapping.h"
#include "../PostProcess/RadialBlur/RadialBlur.h"
#include "../PostProcess/Vignette/Vignette.h"
#include "../PostProcess/ChromaticAberration/ChromaticAberration.h"
#include "../PostProcess/RobertsCross/RobertsCross.h"

#include "FinalPass/FinalPass.h"

void PostProcessManager::Initialize(ID3D11Device* device, uint32_t width, uint32_t height)
{
	{
		// ガウスブラー(ぼかし)
		postProcesses_[static_cast<int>(PostProcessType::GaussianFilterPP)].first =
			std::make_unique<GaussianFilter>(device,
				width, height);
		auto& [name, flag] = postProcesses_[static_cast<int>(PostProcessType::GaussianFilterPP)].second;
		name = TO_STRING_U8(GaussianFilter);
	}
	{
		// 河瀬式ブルーム
		postProcesses_[static_cast<int>(PostProcessType::BloomPP)].first =
			std::make_unique<Bloom>(device,
				width, height);
		auto& [name, flag] = postProcesses_[static_cast<int>(PostProcessType::BloomPP)].second;
		name = TO_STRING_U8(Bloom);
	}
	{
		// ラジアルブラー
		postProcesses_[static_cast<int>(PostProcessType::RadialBlurPP)].first =
			std::make_unique<RadialBlur>(device,
				width, height);
		auto& [name, flag] = postProcesses_[static_cast<int>(PostProcessType::RadialBlurPP)].second;
		name = TO_STRING_U8(RadialBlur);
	}
	{
		// 色収差
		postProcesses_[static_cast<int>(PostProcessType::ChromaticAberrationPP)].first =
			std::make_unique<ChromaticAberration>(device,
				width, height);
		auto& [name, flag] = postProcesses_[static_cast<int>(PostProcessType::ChromaticAberrationPP)].second;
		name = TO_STRING_U8(ChromaticAberration);
	}
	{
		// RobertsCross
		postProcesses_[static_cast<int>(PostProcessType::RobertsCrossPP)].first =
			std::make_unique<RobertsCross>(device,
				width, height);
		auto& [name, flag] = postProcesses_[static_cast<int>(PostProcessType::RobertsCrossPP)].second;
		name = TO_STRING_U8(RobertsCross);
	}
	{
		// 最終パス
		postProcesses_[static_cast<int>(PostProcessType::FinalPassPP)].first =
			std::make_unique<FinalPass>(device,
				width, height);
		auto& [name, flag] = postProcesses_[static_cast<int>(PostProcessType::FinalPassPP)].second;
		name = TO_STRING_U8(FinalPass);
	}

	// ブルーム用
	bloomRenderFrame_ = std::make_unique<FrameBuffer>(device, width, height);
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/AddBloomPS.cso",
		bloomPS_.ReleaseAndGetAddressOf());
	fullscreenQuad_ = std::make_unique<Sprite>(device, L"",
		".\\Data\\Shader\\FullscreenQuadVS.cso");
}

void PostProcessManager::Update(float elapsedTime)
{
	for (size_t i = 0; i < static_cast<int>(PostProcessType::MAX_PostProcessType); ++i)
	{
		// 要素チェック
		if (postProcesses_[i].first == nullptr)continue;

		// 更新処理
		postProcesses_[i].first->Update(elapsedTime);
	}
}

void PostProcessManager::ApplyEffect(RenderContext& rc,
	ID3D11ShaderResourceView** srcSRV)
{
	// 参考:https://docs.unity3d.com/ja/Packages/com.unity.render-pipelines.high-definition@10.5/manual/Post-Processing-Execution-Order.html

	ID3D11DeviceContext* dc = rc.deviceContext;

	// ブルーム作成
	PostProcessBase* bloomPP = GetPostProcess(PostProcessType::BloomPP);
	bloomPP->Render(dc, srcSRV, 0, 1);

	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
	dc->PSSetSamplers(0, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::PointWrap));
	// RobertsCross
	PostProcessBase* robertsCrossPP = GetPostProcess(PostProcessType::RobertsCrossPP);
	robertsCrossPP->Render(dc, srcSRV, 0, 2);
	// ラジアルブラー
	PostProcessBase* radialBlurPP = GetPostProcess(PostProcessType::RadialBlurPP);
	radialBlurPP->Render(dc, robertsCrossPP->GetColorSRV().GetAddressOf(), 0, 1);
	// 色収差
	PostProcessBase* chromaticAberrationPP = GetPostProcess(PostProcessType::ChromaticAberrationPP);
	chromaticAberrationPP->Render(dc, radialBlurPP->GetColorSRV().GetAddressOf(), 0, 1);

	// ブルーム適応
	bloomRenderFrame_->ClearAndActivate(dc);
	{
		ID3D11ShaderResourceView* srvs[] =
		{
			chromaticAberrationPP->GetColorSRV().Get(),
			bloomPP->GetColorSRV().Get()
		};
		// 高輝度抽出し、ぼかしたものを加算描画
		dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
		dc->PSSetSamplers(0, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::BorderPoint));
		fullscreenQuad_->Blit(dc, srvs, 0, _countof(srvs), bloomPS_.Get());
	}
	bloomRenderFrame_->Deactivate(dc);

	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
	dc->PSSetSamplers(0, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::PointWrap));

	// 最終パス
	PostProcessBase* finalPassPP = GetPostProcess(PostProcessType::FinalPassPP);
	finalPassPP->Render(dc, bloomRenderFrame_->GetColorSRV().GetAddressOf(), 0, 1);

	appliedEffectSRV_ = finalPassPP->GetColorSRV();
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
				GuiFlag& guiFlag = postProcesses_[i].second;
				ImGui::Checkbox(guiFlag.first.c_str(), &guiFlag.second);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	for (size_t i = 0; i < static_cast<int>(PostProcessType::MAX_PostProcessType); ++i)
	{
		GuiFlag& guiFlag = postProcesses_[i].second;
		if (guiFlag.second == false)
			continue;

		// 要素チェック
		if (postProcesses_[i].first == nullptr)continue;

		// Gui描画
		postProcesses_[i].first->DrawGui();
	}
}
