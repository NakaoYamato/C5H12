#include "MetaAI.h"

#include "../../Source/Common/Damageable.h"
#include "../../Library/Scene/SceneManager.h"
#include "../../Library/Math/Random.h"

#include "../../Source/Enemy/Wyvern/WyvernActor.h"
#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>

// 開始処理
void MetaAI::Start()
{
}

// 更新処理
void MetaAI::Update(float elapsedTime)
{
	int deadCount = 0;
	for (auto& target : _targetables)
	{
		// 無効なターゲットはスキップ
		if (target.lock() == nullptr)
			continue;

		if (target.lock()->GetFaction() == Targetable::Faction::Enemy)
		{
			auto damageable = target.lock()->GetActor()->GetComponent<Damageable>();
			if (damageable)
			{
				if (damageable->IsDead())
				{
					deadCount++;
				}
			}
		}
		else if (target.lock()->GetFaction() == Targetable::Faction::Player)
		{
			auto damageable = target.lock()->GetActor()->GetComponent<Damageable>();
			if (damageable)
			{
				if (damageable->IsDead())
				{
					_gameOver = true;
				}
			}
        }
	}

	if (deadCount > 0)
		_gameClear = true;

	if (_gameClear)
	{
		if (_INPUT_PRESSED("Start"))
		{
			_inputTimer += elapsedTime;
			if (_inputTimer > _inputHoldTime)
			{
				// シーン遷移
				SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Title");
			}
		}
	}

	TextRenderer::TextDrawData textData;
	textData.type = FontType::MSGothic;
	textData.position = _textPosition;
	textData.color = Vector4::White;
	textData.scale = Vector2(1.0f, 1.0f);
	if (_gameClear)
	{
		// タイトル画面へ戻るメッセージ表示
		if (Input::Instance().GetCurrentInputDevice() == Input::InputType::XboxPad)
			textData.text = L"ゲームクリア：スタートボタン長押しでタイトル画面";
		else
			textData.text = L"ゲームクリア：Tabキー長押しでタイトル画面";
	}
	else if (_gameOver)
	{
		// ゲームオーバーメッセージ表示
		if (Input::Instance().GetCurrentInputDevice() == Input::InputType::XboxPad)
			textData.text = L"ゲームオーバー：スタートボタン長押しでタイトル画面";
		else
			textData.text = L"ゲームオーバー：Tabキー長押しでタイトル画面";
	}
	GetActor()->GetScene()->GetTextRenderer().Draw(textData);
}

// GUI描画
void MetaAI::DrawGui()
{
	ImGui::DragFloat2(u8"テキスト位置", &_textPosition.x, 1.0f, 0.0f, 1920.0f);

	ImGui::Separator();
	ImGui::Checkbox(u8"ゲームクリアフラグ", &_gameClear);
	ImGui::Checkbox(u8"ゲームオーバーフラグ", &_gameOver);
	for (const auto& targetable : _targetables)
	{
		if (auto target = targetable.lock())
		{
			ImGui::Text("Target: %s", target->GetActor()->GetName());
		}
		else
		{
			ImGui::Text("Target: (expired)");
		}
	}

	if (ImGui::Button(u8"モデル読み込み"))
	{
		static auto model = std::make_shared<Model>(Graphics::Instance().GetDevice(), WyvernActor::GetModelFilePath());
	}

	if (ImGui::Button(u8"ワイバーン生成"))
	{
		auto scene = GetActor()->GetScene();
		auto wyvernActor = scene->RegisterActor<WyvernActor>("Wyvern", ActorTag::Enemy);
	}
}

#pragma region ターゲット
/// ターゲット登録
void MetaAI::RegisterTargetable(std::weak_ptr<Targetable> targetable)
{
	if (auto target = targetable.lock())
	{
		_targetables.push_back(targetable);
	}
}

/// ターゲット削除
void MetaAI::RemoveTargetable(std::weak_ptr<Targetable> targetable)
{
	auto it = std::remove_if(_targetables.begin(), _targetables.end(),
		[&targetable](const std::weak_ptr<Targetable>& t) { return t.lock() == targetable.lock(); });
	_targetables.erase(it, _targetables.end());
}

/// ターゲット検索
Targetable* MetaAI::SearchTarget(Targetable::Faction faction, const Vector3& searchPosition, float searchRadius, const std::function<bool(Targetable*)>& searchFanction) const
{
	float searchRadiusSq = searchRadius * searchRadius;
	Targetable* result = nullptr;

	for (const auto& targetable : _targetables)
	{
		if (auto target = targetable.lock())
		{
			// アクターがアクティブでない場合はスキップ
			if (!target->GetActor()->IsActive())
				continue;

			if (target->GetFaction() == faction && target->IsTargetable())
			{
				// 範囲内かチェック
				if (Vector3::LengthSq(target->GetActor()->GetTransform().GetWorldPosition() - searchPosition) > searchRadiusSq)
					continue;

				// サーチ関数が指定されている場合、条件を満たすかチェック
				if (searchFanction)
				{
					if (!searchFanction(target.get()))
						continue;
				}

				// 過去の結果があればヘイト値を比較
				if (result)
				{
					if (target->GetHateValue() < result->GetHateValue())
						continue;
				}

				// ターゲット可能なオブジェクトを見つけた場合、結果を更新
				result = targetable.lock().get();
			}
		}
	}
	return result;
}
/// 特定の位置からランダムな位置を取得
Vector3 MetaAI::GetRandomPositionInRange(const Vector3& center, float range) const
{
	Vector3 randomPosition = center;
	randomPosition.x += Random::RandBias() * range;
	randomPosition.z += Random::RandBias() * range;
	// ランダムな位置が範囲内に収まるように調整
	randomPosition = randomPosition.ClampSphere(center, range);
	return randomPosition;
}
#pragma endregion

#pragma region 開始地点
/// 開始地点登録
void MetaAI::RegisterEntryZone(std::weak_ptr<EntryZone> entryZone)
{
	_entryZones.push_back(entryZone);
}

/// 開始地点削除
void MetaAI::RemoveEntryZone(std::weak_ptr<EntryZone> entryZone)
{
	auto it = std::remove_if(_entryZones.begin(), _entryZones.end(),
		[&entryZone](const std::weak_ptr<EntryZone>& e) { return e.lock() == entryZone.lock(); });
	_entryZones.erase(it, _entryZones.end());
}

/// 指定の位置から最も近い開始地点を検索
EntryZone* MetaAI::SearchNearestEntryZone(Targetable::Faction faction, const Vector3& position) const
{
	EntryZone* result = nullptr;
	float nearestDistanceSq = FLT_MAX;
	for (const auto& entryZoneWeak : _entryZones)
	{
		if (auto entryZone = entryZoneWeak.lock())
		{
			// アクターがアクティブでない場合はスキップ
			if (!entryZone->GetActor()->IsActive())
				continue;
			// 陣営が一致するかチェック
			if (entryZone->GetFaction() == faction)
			{
				float distanceSq = Vector3::LengthSq(entryZone->GetActor()->GetTransform().GetWorldPosition() + entryZone->GetCenter() - position);
				if (distanceSq < nearestDistanceSq)
				{
					nearestDistanceSq = distanceSq;
					result = entryZone.get();
				}
			}
		}
	}
	return result;
}
#pragma endregion

#pragma region ステージ
/// ステージコントローラー登録
void MetaAI::RegisterStageController(std::weak_ptr<StageController> stageController)
{
	_stageControllers.push_back(stageController);
}
/// ステージコントローラー削除
void MetaAI::RemoveStageController(std::weak_ptr<StageController> stageController)
{
	auto it = std::remove_if(_stageControllers.begin(), _stageControllers.end(),
		[&stageController](const std::weak_ptr<StageController>& s) { return s.lock() == stageController.lock(); });
	_stageControllers.erase(it, _stageControllers.end());
}
/// 指定の位置が含まれる開始地点を検索
EntryZone* MetaAI::SearchEntryZoneFromStage(Targetable::Faction faction, int stageIndex) const
{
	for (const auto& stageControllerWeak : _stageControllers)
	{
		if (auto stageController = stageControllerWeak.lock())
		{
			// アクターがアクティブでない場合はスキップ
			if (!stageController->GetActor()->IsActive())
				continue;
			// ステージ番号が一致するかチェック
			if (stageController->GetAreaNumber() == stageIndex)
			{
				// エントリーゾーンリストを走査
				for (auto& entryZone : stageController->GetEntryZones())
				{
					if (!entryZone.lock())
						continue;

					// 陣営が一致するかチェック
					if (entryZone.lock()->GetFaction() == faction)
					{
						return entryZone.lock().get();
					}
				}
			}
		}
	}
	return nullptr;
}
#pragma endregion