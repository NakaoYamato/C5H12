#include "SharpnessController.h"

#include <imgui.h>

float SharpnessController::SharpnessRateTable[WeaponSharpnessLevelMax] = {
	0.50f, // 赤
	0.75f, // 橙
	1.00f, // 黄
	1.05f, // 緑
	1.20f, // 青
	1.32f, // 白
	1.39f, // 紫
};

// 開始処理
void SharpnessController::Start()
{
	// 親からダメージ送信コンポーネント取得
	_damageSender = GetActor()->GetParent()->GetComponent<DamageSender>();
	if (auto damageSender = _damageSender.lock())
	{
		// 攻撃時のコールバック設定
		damageSender->SetOnSendDamageCallback(
			GetName(),
			[&](DamageSender* self, CollisionData& data)
			{
				// 斬れ味消費
				this->ConsumeSharpness(_consumeAmount);
			}
		);
	}
}

// 更新処理
void SharpnessController::Update(float elapsedTime)
{
	if (auto damageSender = _damageSender.lock())
	{
		// 斬れ味倍率設定
		damageSender->SetSharpnessFactor(GetSharpnessRate());
	}
}

// Gui描画
void SharpnessController::DrawGui()
{
	// 斬れ味ゲージGUI描画
	ImGui::Text(u8"現在の斬れ味ゲージ");
	WeaponData::DrawSharpnessGaugeGui(_currentSharpnessGauge, false);
	if (ImGui::Button(u8"斬れ味消費"))
	{
		ConsumeSharpness(10.0f);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"斬れ味回復"))
	{
		RecoverSharpness(10.0f);
	}
	ImGui::Separator();
	ImGui::DragFloat(u8"斬れ味消費量", &_consumeAmount, 0.1f, 0.0f, 100.0f);
	ImGui::Separator();
	if (ImGui::Button(u8"斬れ味ゲージリセット"))
	{
		_currentSharpnessGauge = _baseSharpnessGauge;
	}
	ImGui::Separator();
	ImGui::Text(u8"基本の斬れ味ゲージ");
	WeaponData::DrawSharpnessGaugeGui(_baseSharpnessGauge, false);
}

// 消費
void SharpnessController::ConsumeSharpness(float amount)
{
	// 斬れ味ゲージが無ければ終了
	if (_currentSharpnessGauge.size() == 0)
		return;

	// 後ろから消費していく
	_currentSharpnessGauge[_currentSharpnessGauge.size() - 1] -= amount;
	// 要素が0以下なら次の要素へ
	if (_currentSharpnessGauge[_currentSharpnessGauge.size() - 1] < 0.0f)
	{
		_currentSharpnessGauge.erase(_currentSharpnessGauge.end() - 1);
	}
}

// 回復
void SharpnessController::RecoverSharpness(float amount)
{
	for (size_t i = 0; i < _baseSharpnessGauge.size(); ++i)
	{
		// すでに最大まで回復している場合はスキップ
		if (i < _currentSharpnessGauge.size())
		{
			if (_currentSharpnessGauge[i] >= _baseSharpnessGauge[i])
				continue;
		}
		else
		{
			_currentSharpnessGauge.push_back(0.0f);
		}
		// 回復処理
		_currentSharpnessGauge[i] += amount;
		if (_currentSharpnessGauge[i] > _baseSharpnessGauge[i])
		{
			amount = _currentSharpnessGauge[i] - _baseSharpnessGauge[i];
			_currentSharpnessGauge[i] = _baseSharpnessGauge[i];
		}
		else
		{
			break;
		}
	}
}

// 斬れ味倍率取得
float SharpnessController::GetSharpnessRate()
{
	size_t sharpnessLevel = _currentSharpnessGauge.size();
	sharpnessLevel = std::clamp(sharpnessLevel, size_t(0), size_t(WeaponSharpnessLevelMax));
	return SharpnessRateTable[sharpnessLevel];
}
