#include "ActorManager.h"

#include <imgui.h>

#include "../Converter/ToString.h"


#include "../../Library/Graphics/Graphics.h"

// 更新処理
void ActorManager::Update(float elapsedTime)
{
	// 開始関数の呼び出し
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : startActors_[i])
		{
			actor->Start();
			updateActors_[i].emplace_back(actor);
		}
		startActors_[i].clear();
	}

	// 更新処理
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		float deltaTime = elapsedTime;
		auto& [timeScale, duration] = gameSpeeds_[i];
		if (duration > 0.0f)
		{
			duration -= elapsedTime;
			deltaTime *= timeScale;
		}

		for (auto& actor : updateActors_[i])
		{
			actor->Update(deltaTime);
		}
	}

	// 削除処理
	for (const std::shared_ptr<Actor>& actor : removeActors_)
	{
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			std::vector<std::shared_ptr<Actor>>::iterator itStart = std::find(startActors_[i].begin(), startActors_[i].end(), actor);
			if (itStart != startActors_[i].end())
			{
				startActors_[i].erase(itStart);
			}

			std::vector<std::shared_ptr<Actor>>::iterator itUpdate = std::find(updateActors_[i].begin(), updateActors_[i].end(), actor);
			if (itUpdate != updateActors_[i].end())
			{
				updateActors_[i].erase(itUpdate);
			}

			std::set<std::shared_ptr<Actor>>::iterator itSelection = selectionActors_.find(actor);
			if (itSelection != selectionActors_.end())
			{
				selectionActors_.erase(itSelection);
			}
		}
	}
	removeActors_.clear();
}

// 1秒ごとの更新処理
void ActorManager::FixedUpdate()
{
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : updateActors_[i])
		{
			actor->FixedUpdate();
		}
	}
}

// 当たり判定処理
void ActorManager::Judge()
{
	size_t actorSize{};
	{
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			actorSize += updateActors_[i].size();
		}
	}
	// 総当たりで処理
	for (size_t srcTag = 0; srcTag < static_cast<size_t>(ActorTag::ActorTagMax); ++srcTag)
	{
		for (auto& srcActor : updateActors_[srcTag])
		{
			// 起動チェック
			if (!srcActor->IsActive())
				continue;

			// 当たり判定を行う対象を取得
			for (auto& [dstTag, judgeFlag] : srcActor->GetJudgeTags())
			{
				// src側の当たり判定可能フラグをチェック
				if (!judgeFlag)
					continue;

				const size_t dstTagIndex = static_cast<size_t>(dstTag);
				// 上下関係確認
				if (srcTag > dstTagIndex)
					continue;

				for (auto& dstActor : updateActors_[dstTagIndex])
				{
					// 起動チェック
					if (!dstActor->IsActive())
						continue;

					// 同じアクターか確認
					if (srcActor.get() == dstActor.get())
						continue;

					// dstがsrcと当たり判定を行うか確認
					if (dstActor->GetJudgeTags().at(static_cast<ActorTag>(srcTag)) == false)
						continue;

					srcActor->Judge(dstActor.get());
				}

			}
		}
	}
}

// 描画の前処理
void ActorManager::RenderPreprocess(RenderContext& rc)
{
	// rcにパラメータを設定
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : updateActors_[i])
		{
			actor->RenderPreprocess(rc);
		}
	}
}

// 描画処理
void ActorManager::Render(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	const RenderState* renderState = rc.renderState;

	// レンダーステート設定
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullBack));

	// サンプラーステート設定
	{
		ID3D11SamplerState* samplerStates[] =
		{
			renderState->GetSamplerState(SamplerState::PointWrap),
			renderState->GetSamplerState(SamplerState::PointClamp),
			renderState->GetSamplerState(SamplerState::LinearWrap),
			renderState->GetSamplerState(SamplerState::LinearClamp)
		};
		dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);
	}

	ConstantBufferManager* cbManager = Graphics::Instance().GetConstantBufferManager();
	// シーン定数バッファ、ライト定数バッファの更新
	cbManager->Update(rc);
	// シーン定数バッファの設定
	cbManager->SetCB(dc, 0, ConstantBufferType::SceneCB, ConstantUpdateTarget::ALL);
	// ライト定数バッファの設定
	cbManager->SetCB(dc, 3, ConstantBufferType::LightCB, ConstantUpdateTarget::ALL);

	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : updateActors_[i])
		{
			actor->Render(rc);
#ifdef _DEBUG
			actor->DebugRender(rc);
#endif // _DEBUG
		}
	}
}

// 影描画処理
void ActorManager::CastShadow(const RenderContext& rc)
{
	for (size_t i = static_cast<size_t>(ActorTag::Stage); i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : updateActors_[i])
		{
			actor->CastShadow(rc);
		}
	}
}

// 3D描画後の描画処理
void ActorManager::DelayedRender(RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	const RenderState* renderState = rc.renderState;

	// レンダーステート設定
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullBack));

	// サンプラーステート設定
	{
		ID3D11SamplerState* samplerStates[] =
		{
			renderState->GetSamplerState(SamplerState::PointWrap),
			renderState->GetSamplerState(SamplerState::PointClamp),
			renderState->GetSamplerState(SamplerState::LinearWrap),
			renderState->GetSamplerState(SamplerState::LinearClamp)
		};
		dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);
	}

	for (size_t i = static_cast<size_t>(ActorTag::Stage); i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : updateActors_[i])
		{
			actor->DelayedRender(rc);
		}
	}
}

// Gui描画
void ActorManager::DrawGui()
{
	// 登録しているオブジェクトの一覧
	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
	if (ImGui::Begin(u8"ゲームオブジェクト一覧"))
	{
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			if (ImGui::TreeNodeEx(ToString<ActorTag>(i).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (auto& actor : updateActors_[i])
				{
					ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_DefaultOpen;
					if (ImGui::TreeNodeEx(actor.get(), nodeFlags, actor->GetName()))
					{
						// ダブルクリックで選択
						if (ImGui::IsItemClicked())
						{
							showGuiObjects_[actor->GetName()] = true;
						}

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
	ImGui::PopStyleColor();

	// 選んでいるオブジェクトの削除用コンテナ
	std::vector<std::string> eraseNames;
	if (ImGui::Begin(u8"選択中のゲームオブジェクト"))
	{
		static ImGuiTabBarFlags tab_bar_flags =
			ImGuiTabBarFlags_AutoSelectNewTabs |
			ImGuiTabBarFlags_Reorderable |
			ImGuiTabBarFlags_FittingPolicyResizeDown;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			// 選んでいるオブジェクトのGUI描画
			for (auto& [name, showGui] : showGuiObjects_)
			{
				if (showGui == false)
				{
					eraseNames.push_back(name);
					continue;
				}

				Actor* object = FindByName(name).get();
				if (object)
				{
					if (ImGui::BeginTabItem(name.c_str(), &showGui))
					{
						object->DrawGui();

						ImGui::EndTabItem();
					}
				}
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();

	// 選んでいるオブジェクトの削除
	for (auto& name : eraseNames)
	{
		showGuiObjects_.erase(name);
	}
}

/// 指定要素の取得
ActorMap& ActorManager::FindByTag(ActorTag tag)
{
	return updateActors_[static_cast<size_t>(tag)];
}
ActorMap& ActorManager::FindByTagInStartActors(ActorTag tag)
{
	return startActors_[static_cast<size_t>(tag)];
}
std::shared_ptr<Actor> ActorManager::FindByName(const std::string& name, ActorTag tag)
{
	if (tag != ActorTag::ActorTagMax)
	{
		for (auto& actor : updateActors_[static_cast<size_t>(tag)])
		{
			if (name == actor->GetName())
				return actor;
		}
	}
	else
	{
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			for (auto& actor : updateActors_[i])
			{
				if (name == actor->GetName())
					return actor;
			}
		}
	}

	// 要素がなければnullptr
	return nullptr;
}
std::shared_ptr<Actor> ActorManager::FindByNameFromStartActor(const std::string& name, ActorTag tag)
{
	if (tag != ActorTag::ActorTagMax)
	{
		for (auto& actor : startActors_[static_cast<size_t>(tag)])
		{
			if (name == actor->GetName())
				return actor;
		}
	}
	else
	{
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			for (auto& actor : startActors_[i])
			{
				if (name == actor->GetName())
					return actor;
			}
		}
	}

	// 要素がなければnullptr
	return nullptr;
}

// 要素の全削除
void ActorManager::Clear()
{
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		startActors_[i].clear();
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		updateActors_[i].clear();

	selectionActors_.clear();
	removeActors_.clear();
}

void ActorManager::Register(std::shared_ptr<Actor> actor, ActorTag tag)
{
#ifdef _DEBUG
	if (FindByName(actor->GetName(), tag))
		assert(!"名前の重複");
#endif
	// 当たり判定フラグを設定
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		actor->SetJudgeTagFlag(static_cast<ActorTag>(i), true);
	}
	actor->SetJudgeTagFlag(ActorTag::DrawContextParameter, false);
	//　startActorsに登録
	FindByTagInStartActors(tag).emplace_back(actor);
}

// 指定要素の削除
void ActorManager::Remove(std::shared_ptr<Actor> actor)
{
	removeActors_.insert(actor);
}
void ActorManager::Remove(const std::string& name)
{
	Remove(FindByName(name));
}
void ActorManager::Remove(ActorTag tag)
{
	ActorMap& actorContainer = FindByTag(tag);
	for (auto& actor : actorContainer)
	{
		Remove(actor);
	}
}
// ゲームスピードの設定
void ActorManager::SetGameSpeed(ActorTag tag, float scale, float duration)
{
	gameSpeeds_[static_cast<size_t>(tag)] = std::make_pair(scale, duration);
}
