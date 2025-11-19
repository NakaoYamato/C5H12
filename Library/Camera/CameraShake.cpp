#include "CameraShake.h"
#include <algorithm>
#include <cmath>

#include <imgui.h>

CameraShakeManager::CameraShakeManager()
    : _rng(std::random_device{}())
    , _dist(-1.0f, 1.0f)
{
}

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

void CameraShakeManager::StartShake(const CameraShakeInfo& info)
{
    // ランダムな軸を生成（揺れのベースとなる方向）
    Vector3 seed(RandomFloat(), RandomFloat(), RandomFloat());

    // ベクトル正規化（Vector3の実装に依存。Normalize関数がない場合は自作してください）
    // seed.Normalize(); 

    _activeShakes.emplace_back(info, seed);
}

void CameraShakeManager::StopAll()
{
    _activeShakes.clear();
}

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

Vector3 CameraShakeManager::GetTotalOffset(const Vector3& listenerPosition) const
{
    Vector3 totalOffset = { 0.0f, 0.0f, 0.0f };

    for (const auto& shake : _activeShakes)
    {
        // 1. 進行度 (0.0 -> 1.0)
        float progress = 0.0f;
        if (shake.info.duration > 0.0f)
        {
            progress = std::clamp(shake.time / shake.info.duration, 0.0f, 1.0f);
        }

        // 2. 基本強度の計算 (Envelope)
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

        // 3. 距離減衰 (Spatial Attenuation)
        if (shake.info.usePosition)
        {
            // Vector3の減算とLength()を想定
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
            // else (dist <= nearDistance) の場合は減衰なし(1.0)
        }

        // 強度がほぼゼロなら計算スキップ
        if (intensity <= 0.001f) continue;

        // 4. 振動波形の生成 (Perlin Noiseの簡易代替: Sin波 + ランダムシード)
        // 時間経過 × 周波数 で波を作る
        float noise = std::sinf(shake.time * shake.info.frequency * 6.28f);

        // 毎回ランダムなジッターを加えると、より「ガガガッ」というノイズ感が出る
        // ここでは簡易的にシードベクトルとnoiseを掛け合わせる

        Vector3 offset;
        offset.x = shake.seedVector.x * noise; // 必要に応じてここでさらに sin(time * freq * 1.5) などずらすと複雑になる
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

float CameraShakeManager::RandomFloat()
{
    return _dist(_rng);
}