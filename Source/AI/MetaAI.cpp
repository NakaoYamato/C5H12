#include "MetaAI.h"

#include "../../Source/Common/Damageable.h"
#include "../../Library/Scene/SceneManager.h"
#include "../../Library/Math/Random.h"

#include <imgui.h>

// 開始処理
void MetaAI::Start()
{
}

// 更新処理
void MetaAI::Update(float elapsedTime)
{
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
					_gameClear = true;
				}
			}
		}
	}

	if (_gameClear)
	{
		_clearMovieTimer += elapsedTime;
		if (_clearMovieTimer > _clearMovieTime)
		{
			// シーン遷移
			SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Result");
		}
	}
}

// GUI描画
void MetaAI::DrawGui()
{
	ImGui::Checkbox(u8"ゲームクリアフラグ", &_gameClear);
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
}

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
