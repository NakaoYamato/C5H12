#include "ActorManager.h"

#include <functional>
#include <imgui.h>

#include "../Algorithm/Converter.h"

#include "../../Library/JobSystem/JobSystem.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

// 更新処理
void ActorManager::Update(float elapsedTime)
{
	//----------------------------------------------------------------
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
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	// 更新処理
	if (JobSystem::Instance().UseMultiThread())
	{
		// 全体の計算時間
		ProfileScopedSection_2(0, ActorManager, ImGuiControl::Profiler::Dark);

		std::vector<std::future<void>> jobResults;
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
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	// 削除処理
	UpdateRemove();
	//----------------------------------------------------------------
}

// 遅延更新処理
void ActorManager::LateUpdate(float elapsedTime)
{
	//----------------------------------------------------------------
	// LateUpdate処理
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
			ProfileScopedSection_3(0, actor->GetName(), ImGuiControl::Profiler::Green);

			actor->LateUpdate(deltaTime);
		}
	}
	//----------------------------------------------------------------
}

// 一定間隔の更新処理
void ActorManager::FixedUpdate()
{
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : _updateActors[i])
		{
			ProfileScopedSection_3(0, actor->GetName(), ImGuiControl::Profiler::Red);
			actor->FixedUpdate();
		}
	}
}

// 描画処理
void ActorManager::Render(const RenderContext& rc)
{
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : _updateActors[i])
		{
			ProfileScopedSection_3(0, actor->GetName(), ImGuiControl::Profiler::Yellow);
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
			ProfileScopedSection_3(0, actor->GetName(), ImGuiControl::Profiler::Blue);
			actor->CastShadow(rc);
		}
	}
}

// 3D描画後の描画処理
void ActorManager::DelayedRender(RenderContext& rc)
{
	for (size_t i = static_cast<size_t>(ActorTag::Stage); i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		for (auto& actor : _updateActors[i])
		{
			ProfileScopedSection_3(0, actor->GetName(), ImGuiControl::Profiler::Purple);
			actor->DelayedRender(rc);
		}
	}
}

// Gui描画
void ActorManager::DrawGui()
{
	// アクターのGUI表示
	std::function<void(std::shared_ptr<Actor>)>DrawActorGui = [&](std::shared_ptr<Actor> actor) -> void
		{
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_DefaultOpen;
			// 子供がいるなら矢印表示
			if (actor->GetChildren().size() > 0)
				nodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
			
			if (ImGui::TreeNodeEx(actor.get(), nodeFlags, actor->GetName()))
			{
				actor->SetIsDrawingHierarchy(false);

				// ダブルクリックで選択
				if (ImGui::IsItemClicked())
				{
					_showGuiObj = actor->GetName();
				}

				// 子供の表示
				for (auto& child : actor->GetChildren())
				{
					DrawActorGui(child);
				}

				ImGui::TreePop();
			}
		};

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
					// 親が存在するなら親の方で表示されているのでスキップ
					if (actor->GetParent()) break;

					DrawActorGui(actor);
				}

				ImGui::TreePop();
			}
		}
		// GUIが埋まってしまう問題を解決するためのスペース
		ImGui::Dummy(ImVec2(0.0f, 100.0f));
	}
	ImGui::End();
	ImGui::PopStyleColor();

	if (ImGui::Begin(u8"インスペクター"))
	{
		// 選んでいるオブジェクトのGUI描画
		Actor* object = FindByName(_showGuiObj).get();
		if (object)
		{
			// 親の名前表示
			if (object->GetParent())
			{
				ImGui::Text(object->GetParent()->GetName());
				ImGui::Text((u8"└" + _showGuiObj).c_str());
			}
			else
			{
				ImGui::Text(_showGuiObj.c_str());
			}

			object->SetIsDrawingHierarchy(true);

			object->DrawGui();
		}
		// GUIが埋まってしまう問題を解決するためのスペース
		ImGui::Dummy(ImVec2(0.0f, 100.0f));
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
	}

	// 要素がなければstartActorsから検索
	return FindByNameFromStartActor(name, tag);
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
	for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
	{
		Remove(static_cast<ActorTag>(i));
	}
	UpdateRemove();
}

void ActorManager::Register(std::shared_ptr<Actor> actor, ActorTag tag)
{
#ifdef _DEBUG
	if (FindByName(actor->GetName(), tag))
	{
		Debug::Output::String(L"名前の重複\n");
		std::string str = actor->GetName();
		str += "_dup";
		actor->SetName(str.c_str());
	}
#endif
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);
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

/// 削除更新処理
void ActorManager::UpdateRemove()
{
	for (const std::shared_ptr<Actor>& actor : _removeActors)
	{
		actor->Deleted();

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
		}
	}
	_removeActors.clear();
}
