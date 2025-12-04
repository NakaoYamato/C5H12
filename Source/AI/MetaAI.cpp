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
