#include "PostProcessManager.h"

#include<imgui.h>

#include "../Converter/ToString.h"
#include "../ResourceManager/GpuResourceManager.h"

#include "../PostProcess/GlowExtraction/GlowExtraction.h"
#include "../PostProcess/GaussianFilter/GaussianFilter.h"
#include "../PostProcess/RadialBlur/RadialBlur.h"
#include "../PostProcess/ChromaticAberration/ChromaticAberration.h"
#include "../PostProcess/RobertsCross/RobertsCross.h"
#include "../PostProcess/DepthOfField/DepthOfField.h"
#include "../PostProcess/FXAA/FXAA.h"

#include "FinalPass/FinalPass.h"

//#define _USE_DOF

void PostProcessManager::Initialize(ID3D11Device* device, uint32_t width, uint32_t height)
{
	{
		// ブルーム用高輝度抽出
		_postProcesses[static_cast<int>(PostProcessType::BloomGlowExtractionPP)].first =
			std::make_unique<GlowExtraction>(device,
				width, height);
		auto& [name, flag] = _postProcesses[static_cast<int>(PostProcessType::BloomGlowExtractionPP)].second;
		name = u8"ブルーム用高輝度抽出";
	}
	{
		// ブルーム用ぼかし
		_postProcesses[static_cast<int>(PostProcessType::BloomGradationPP)].first =
			std::make_unique<GaussianFilter>(device,
				width, height);
		auto& [name, flag] = _postProcesses[static_cast<int>(PostProcessType::BloomGradationPP)].second;
		name = u8"ブルーム用ぼかし";
	}
	{
		// 被写体深度用ぼかし
		_postProcesses[static_cast<int>(PostProcessType::DepthOfFieldGradationPP)].first =
			std::make_unique<GaussianFilter>(device,
				width, height);
		auto& [name, flag] = _postProcesses[static_cast<int>(PostProcessType::DepthOfFieldGradationPP)].second;
		name = u8"被写体深度用ぼかし";
	}
	{
		// 被写体深度
		_postProcesses[static_cast<int>(PostProcessType::DepthOfFieldPP)].first =
			std::make_unique<DepthOfField>(device,
				width, height);
		auto& [name, flag] = _postProcesses[static_cast<int>(PostProcessType::DepthOfFieldPP)].second;
		name = u8"被写体深度";
	}
	{
		// ラジアルブラー
		_postProcesses[static_cast<int>(PostProcessType::RadialBlurPP)].first =
			std::make_unique<RadialBlur>(device,
				width, height);
		auto& [name, flag] = _postProcesses[static_cast<int>(PostProcessType::RadialBlurPP)].second;
		name = _TO_STRING_U8(RadialBlur);
	}
	{
		// 色収差
		_postProcesses[static_cast<int>(PostProcessType::ChromaticAberrationPP)].first =
			std::make_unique<ChromaticAberration>(device,
				width, height);
		auto& [name, flag] = _postProcesses[static_cast<int>(PostProcessType::ChromaticAberrationPP)].second;
		name = u8"色収差";
	}
	{
		// RobertsCross
		_postProcesses[static_cast<int>(PostProcessType::RobertsCrossPP)].first =
			std::make_unique<RobertsCross>(device,
				width, height);
		auto& [name, flag] = _postProcesses[static_cast<int>(PostProcessType::RobertsCrossPP)].second;
		name = u8"RobertsCross";
	}
	{
		// アンチエイリアス
		_postProcesses[static_cast<int>(PostProcessType::FXAAPP)].first =
			std::make_unique<FXAA>(device,
				width, height);
		auto& [name, flag] = _postProcesses[static_cast<int>(PostProcessType::FXAAPP)].second;
		name = u8"アンチエイリアス";
	}
	{
		// 最終パス
		_postProcesses[static_cast<int>(PostProcessType::FinalPassPP)].first =
			std::make_unique<FinalPass>(device,
				width, height);
		auto& [name, flag] = _postProcesses[static_cast<int>(PostProcessType::FinalPassPP)].second;
		name = _TO_STRING_U8(FinalPass);
	}

	// ブルーム用
	_bloomRenderFrame = std::make_unique<FrameBuffer>(device, width, height);
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/AddBloomPS.cso",
		_bloomPS.ReleaseAndGetAddressOf());
	_fullscreenQuad = std::make_unique<Sprite>(device, L"",
		".\\Data\\Shader\\FullscreenQuadVS.cso");
}

void PostProcessManager::Update(float elapsedTime)
{
	for (size_t i = 0; i < static_cast<int>(PostProcessType::MAX_PostProcessType); ++i)
	{
		// 要素チェック
		if (_postProcesses[i].first == nullptr)continue;

		// 更新処理
		_postProcesses[i].first->Update(elapsedTime);
	}
}

void PostProcessManager::ApplyEffect(RenderContext& rc,
	ID3D11ShaderResourceView* colorSRV,
	ID3D11ShaderResourceView* depthSRV)
{
	// 参考:https://docs.unity3d.com/ja/Packages/com.unity.render-pipelines.high-definition@10.5/manual/Post-Processing-Execution-Order.html

	ID3D11DeviceContext* dc = rc.deviceContext;

#ifdef _USE_DOF
	// 被写体深度用ぼかし作成
	PostProcessBase* dofGradationPP = GetPostProcess(PostProcessType::DepthOfFieldGradationPP);
	dofGradationPP->Render(dc, &colorSRV, 0, 1);
	// 被写体深度
	PostProcessBase* dofPP = GetPostProcess(PostProcessType::DepthOfFieldPP);
	{
		ID3D11ShaderResourceView* srv[] =
		{
			colorSRV, depthSRV, dofGradationPP->GetColorSRV().Get()
		};
		dofPP->Render(dc, srv, 0, _countof(srv));
	}
#endif

	// 高輝度抽出
	PostProcessBase* glowPP = GetPostProcess(PostProcessType::BloomGlowExtractionPP);
#ifdef _USE_DOF
	glowPP->Render(dc, dofPP->GetColorSRV().GetAddressOf(), 0, 1);
#else
	glowPP->Render(dc, &colorSRV, 0, 1);
#endif

	// ブルーム用ぼかし作成
	PostProcessBase* bloomGradationPP = GetPostProcess(PostProcessType::BloomGradationPP);
	bloomGradationPP->Render(dc, glowPP->GetColorSRV().GetAddressOf(), 0, 1);

	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
	// RobertsCross
	PostProcessBase* robertsCrossPP = GetPostProcess(PostProcessType::RobertsCrossPP);
	{
		ID3D11ShaderResourceView* srv[] =
		{
#ifdef _USE_DOF
			dofPP->GetColorSRV().Get(), depthSRV,
#else
			colorSRV, depthSRV,
#endif
		};
		robertsCrossPP->Render(dc, srv, 0, _countof(srv));
	}
	// ラジアルブラー
	PostProcessBase* radialBlurPP = GetPostProcess(PostProcessType::RadialBlurPP);
	radialBlurPP->Render(dc, robertsCrossPP->GetColorSRV().GetAddressOf(), 0, 1);
	// 色収差
	PostProcessBase* chromaticAberrationPP = GetPostProcess(PostProcessType::ChromaticAberrationPP);
	chromaticAberrationPP->Render(dc, radialBlurPP->GetColorSRV().GetAddressOf(), 0, 1);

	// ブルーム適応
	_bloomRenderFrame->ClearAndActivate(dc);
	{
		ID3D11ShaderResourceView* srvs[] =
		{
			chromaticAberrationPP->GetColorSRV().Get(),
			bloomGradationPP->GetColorSRV().Get()
		};
		// 高輝度抽出し、ぼかしたものを加算描画
		dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
		dc->PSSetSamplers(0, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::BorderPoint));
		_fullscreenQuad->Blit(dc, srvs, 0, _countof(srvs), _bloomPS.Get());
	}
	_bloomRenderFrame->Deactivate(dc);

	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
	dc->PSSetSamplers(0, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::PointWrap));

	// 最終パス
	PostProcessBase* finalPassPP = GetPostProcess(PostProcessType::FinalPassPP);
	finalPassPP->Render(dc, _bloomRenderFrame->GetColorSRV().GetAddressOf(), 0, 1);

	// アンチエイリアス
	PostProcessBase* fxaaPP = GetPostProcess(PostProcessType::FXAAPP);
	fxaaPP->Render(dc, finalPassPP->GetColorSRV().GetAddressOf(), 0, 1);

	_appliedEffectSRV = fxaaPP->GetColorSRV();
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
				GuiFlag& guiFlag = _postProcesses[i].second;
				ImGui::Checkbox(guiFlag.first.c_str(), &guiFlag.second);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	for (size_t i = 0; i < static_cast<int>(PostProcessType::MAX_PostProcessType); ++i)
	{
		GuiFlag& guiFlag = _postProcesses[i].second;
		if (guiFlag.second == false)
			continue;

		// 要素チェック
		if (_postProcesses[i].first == nullptr)continue;

		// Gui描画
		if (ImGui::Begin(_postProcesses[i].second.first.c_str()))
		{
			if (ImGui::Button("reset"))
				_postProcesses[i].first->ClearData();
			ImGui::Separator();
			_postProcesses[i].first->DrawGui();

			if (ImGui::TreeNode("Resource"))
			{
				static float textureSize = 512.0f;
				ImGui::DragFloat("TextureSize", &textureSize);
				ImGui::Image(_postProcesses[i].first->GetColorSRV().Get(),
					{ textureSize ,textureSize });

				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
}
