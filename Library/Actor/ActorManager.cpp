#include "ActorManager.h"

#include <imgui.h>

#include "../Converter/ToString.h"

#include "../../Library/JobSystem/JobSystem.h"

#include "../../Library/Graphics/Graphics.h"

// 更新処理
void ActorManager::Update(float elapsedTime)
{
	// 開始関数の呼び出し
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : _startActors[i])
		{
			actor->Start();
			_updateActors[i].emplace_back(actor);
		}
		_startActors[i].clear();
	}

	if (JobSystem::Instance().UseMultiThread())
	{
		// 全体の計算時間
		ProfileScopedSection_2(0, ActorManager, ImGuiControl::Profiler::Dark);

		std::vector<std::future<void>> jobResults;
		// 更新処理
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			float deltaTime = elapsedTime;
			auto& [timeScale, duration] = _gameSpeeds[i];
			if (duration > 0.0f)
			{
				duration -= elapsedTime;
				deltaTime *= timeScale;
			}

			for (auto& actor : _updateActors[i])
			{
				jobResults.emplace_back(JobSystem::Instance().EnqueueJob(actor->GetName(),
					ImGuiControl::Profiler::Color::Blue,
					[&]()
					{
						actor->Update(deltaTime);
					}
				));
			}
		}
		// すべてのジョブの終了を待機
		for (auto& result : jobResults)
		{
			result.get();
		}
	}
	else
	{
		// 更新処理
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			float deltaTime = elapsedTime;
			auto& [timeScale, duration] = _gameSpeeds[i];
			if (duration > 0.0f)
			{
				duration -= elapsedTime;
				deltaTime *= timeScale;
			}

			for (auto& actor : _updateActors[i])
			{
				// 各アクターの計算時間
				ProfileScopedSection_3(0, actor->GetName(), ImGuiControl::Profiler::Dark);

				actor->Update(deltaTime);
			}
		}
	}

	// 削除処理
	for (const std::shared_ptr<Actor>& actor : _removeActors)
	{
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			std::vector<std::shared_ptr<Actor>>::iterator itStart = std::find(_startActors[i].begin(), _startActors[i].end(), actor);
			if (itStart != _startActors[i].end())
			{
				_startActors[i].erase(itStart);
			}

			std::vector<std::shared_ptr<Actor>>::iterator itUpdate = std::find(_updateActors[i].begin(), _updateActors[i].end(), actor);
			if (itUpdate != _updateActors[i].end())
			{
				_updateActors[i].erase(itUpdate);
			}

			std::set<std::shared_ptr<Actor>>::iterator itSelection = _selectionActors.find(actor);
			if (itSelection != _selectionActors.end())
			{
				_selectionActors.erase(itSelection);
			}
		}
	}
	_removeActors.clear();
}

/// 一定間隔の更新処理
void ActorManager::FixedUpdate()
{
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : _updateActors[i])
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
			actorSize += _updateActors[i].size();
		}
	}
	// 総当たりで処理
	for (size_t srcTag = 0; srcTag < static_cast<size_t>(ActorTag::ActorTagMax); ++srcTag)
	{
		for (auto& srcActor : _updateActors[srcTag])
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

				for (auto& dstActor : _updateActors[dstTagIndex])
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
		for (auto& actor : _updateActors[i])
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
		for (auto& actor : _updateActors[i])
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
		for (auto& actor : _updateActors[i])
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
				for (auto& actor : _updateActors[i])
				{
					ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_DefaultOpen;
					if (ImGui::TreeNodeEx(actor.get(), nodeFlags, actor->GetName()))
					{
						actor->SetDrawHierarchyFlag(false);

						// ダブルクリックで選択
						if (ImGui::IsItemClicked())
						{
							_showGuiObj = actor->GetName();
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

	if (ImGui::Begin(u8"インスペクター"))
	{
		// 選んでいるオブジェクトのGUI描画
		Actor* object = FindByName(_showGuiObj).get();
		if (object)
		{
			ImGui::Text(_showGuiObj.c_str());

			object->SetDrawHierarchyFlag(true);

			object->DrawGui();
		}
	}
	ImGui::End();
}

/// 指定要素の取得
ActorMap& ActorManager::FindByTag(ActorTag tag)
{
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);
	return _updateActors[static_cast<size_t>(tag)];
}
ActorMap& ActorManager::FindByTagInStartActors(ActorTag tag)
{
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);
	return _startActors[static_cast<size_t>(tag)];
}
std::shared_ptr<Actor> ActorManager::FindByName(const std::string& name, ActorTag tag)
{
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);
	if (tag != ActorTag::ActorTagMax)
	{
		for (auto& actor : _updateActors[static_cast<size_t>(tag)])
		{
			if (name == actor->GetName())
				return actor;
		}
	}
	else
	{
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			for (auto& actor : _updateActors[i])
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
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);
	if (tag != ActorTag::ActorTagMax)
	{
		for (auto& actor : _startActors[static_cast<size_t>(tag)])
		{
			if (name == actor->GetName())
				return actor;
		}
	}
	else
	{
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			for (auto& actor : _startActors[i])
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
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		_startActors[i].clear();
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		_updateActors[i].clear();

	_selectionActors.clear();
	_removeActors.clear();
}

void ActorManager::Register(std::shared_ptr<Actor> actor, ActorTag tag)
{
#ifdef _DEBUG
	if (FindByName(actor->GetName(), tag))
		assert(!"名前の重複");
#endif
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);
	// 当たり判定フラグを設定
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		actor->SetJudgeTagFlag(static_cast<ActorTag>(i), true);
	}
	actor->SetJudgeTagFlag(ActorTag::DrawContextParameter, false);
	//　startActorsに登録
	_startActors[static_cast<size_t>(tag)].emplace_back(actor);
}

// 指定要素の削除
void ActorManager::Remove(std::shared_ptr<Actor> actor)
{
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);
	_removeActors.insert(actor);
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
	_gameSpeeds[static_cast<size_t>(tag)] = std::make_pair(scale, duration);
}
