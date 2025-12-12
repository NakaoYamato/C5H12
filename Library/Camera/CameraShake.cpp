#include "CameraShake.h"
#include <algorithm>
#include <cmath>

#include "../../Library/Math/Random.h"

#include <imgui.h>

CameraShakeManager::CameraShakeManager()
    : _rng(std::random_device{}())
    , _dist(-1.0f, 1.0f)
{
}

// 更新処理
void CameraShakeManager::Update(float elapsedTime)
{
    auto it = _activeShakes.begin();
    while (it != _activeShakes.end())
    {
        it->time += elapsedTime;

        // 終了判定（減衰なしタイプ以外で、時間が超過したら削除）
        if (it->info.decayType != ShakeDecayType::None && it->time >= it->info.duration)
        {
            it = _activeShakes.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// シェイクの開始
void CameraShakeManager::StartShake(const CameraShakeInfo& info)
{
    // ランダムな軸を生成（揺れのベースとなる方向）
    Vector3 seed(Random::RandBias(), Random::RandBias(), Random::RandBias());

    seed.Normalize(); 

    _activeShakes.emplace_back(info, seed);
}

// すべての揺れを停止
void CameraShakeManager::StopAll()
{
    _activeShakes.clear();
}

// GUI描画
void CameraShakeManager::DrawGui()
{
    ImGui::DragFloat(u8"デバッグ用最大深度", &_debugAmplitude, 0.1f);
	ImGui::DragFloat(u8"デバッグ用周波数", &_debugFrequency, 0.1f);
	ImGui::DragFloat(u8"デバッグ用持続時間", &_debugDuration, 0.1f);
	ImGui::DragFloat3(u8"デバッグ用方向マスク", &_debugDirectionMask.x, 0.1f);
	ImGui::Combo(u8"デバッグ用減衰タイプ", (int*)&_debugDecayType, u8"線形\0指数\0");
	if (ImGui::Button(u8"デバッグ用シェイク開始"))
	{
		CameraShakeInfo debugShake;
		debugShake.amplitude = _debugAmplitude;
		debugShake.frequency = _debugFrequency;
		debugShake.duration = _debugDuration;
		debugShake.directionMask = _debugDirectionMask;
		debugShake.decayType = _debugDecayType;
		StartShake(debugShake);
	}
}

// 現在の揺れオフセットを取得
Vector3 CameraShakeManager::GetTotalOffset(const Vector3& listenerPosition) const
{
    Vector3 totalOffset = { 0.0f, 0.0f, 0.0f };

    for (const auto& shake : _activeShakes)
    {
        // 進行度 (0.0 -> 1.0)
        float progress = 0.0f;
        if (shake.info.duration > 0.0f)
        {
            progress = std::clamp(shake.time / shake.info.duration, 0.0f, 1.0f);
        }

        // 基本強度の計算
        float intensity = 1.0f;
        switch (shake.info.decayType)
        {
        case ShakeDecayType::Linear:
            intensity = 1.0f - progress;
            break;
        case ShakeDecayType::Exponential:
            // 2乗減衰で、衝撃の瞬発力を表現
            intensity = (1.0f - progress) * (1.0f - progress);
            break;
        case ShakeDecayType::None:
            intensity = 1.0f;
            break;
        }

        // 距離減衰
        if (shake.info.usePosition)
        {
            Vector3 diff = listenerPosition - shake.info.sourcePosition;
            float dist = diff.Length();

            if (dist > shake.info.reachDistance)
            {
                intensity = 0.0f;
            }
            else if (dist > shake.info.nearDistance)
            {
                // near ～ reach の間で 1.0 -> 0.0 に減衰
                float range = shake.info.reachDistance - shake.info.nearDistance;
                float t = (dist - shake.info.nearDistance) / range;
                intensity *= (1.0f - t);
            }
        }

        // 強度がほぼゼロなら計算スキップ
        if (intensity <= 0.001f) continue;

        // 振動波形の生成
        float noise = std::sinf(shake.time * shake.info.frequency * 6.28f);

        Vector3 offset;
        offset.x = shake.seedVector.x * noise;
        offset.y = shake.seedVector.y * noise;
        offset.z = shake.seedVector.z * noise;

        // 振幅と強度を適用
        offset = offset * shake.info.amplitude * intensity;

        // 方向マスク適用
        offset.x *= shake.info.directionMask.x;
        offset.y *= shake.info.directionMask.y;
        offset.z *= shake.info.directionMask.z;

        totalOffset += offset;
    }

    return totalOffset;
}
