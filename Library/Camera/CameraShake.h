#pragma once
#include "../Math/Vector.h" // ユーザー環境のVector.h
#include <vector>
#include <list>
#include <random>

/// <summary>
/// シェイクの減衰タイプ
/// </summary>
enum class ShakeDecayType
{
    Linear,     // 線形減衰
    Exponential,// 指数減衰
    None,       // 減衰なし
};

// シェイク発生パラメータ
struct CameraShakeInfo
{
    // 最大振幅（揺れの強さ）
    float amplitude = 1.0f;
    // 持続時間（秒）
    float duration = 0.5f;
    // 周波数（揺れの速さ・細かさ）
    float frequency = 20.0f;
    // 揺れる軸の指定（(0,1,0)なら縦揺れのみ）
    Vector3 directionMask = { 1,1,1 };

    // 減衰タイプ
    ShakeDecayType decayType = ShakeDecayType::Exponential;

    // 発生源座標を使用するか
    bool usePosition = false;
    // 発生源のワールド座標
    Vector3 sourcePosition = { 0,0,0 };
    // 揺れが届く限界距離
    float reachDistance = 100.0f;
    // 最大強度で揺れる距離（これより近いと減衰しない）
    float nearDistance = 10.0f;
};

// 個別の揺れインスタンス（内部管理用）
struct ShakeInstance
{
    CameraShakeInfo info;
    // 経過時間
    float time = 0.0f;
    // ランダムシード用ベクトル
    Vector3 seedVector;
    bool isFinished = false;

    ShakeInstance(const CameraShakeInfo& info, const Vector3& seed)
        : info(info), seedVector(seed), time(0.0f) {
    }
};

// カメラシェイク管理クラス
class CameraShakeManager
{
public:
    CameraShakeManager();
    ~CameraShakeManager() = default;

    // 更新処理
    void Update(float elapsedTime);

    // シェイクの開始
    void StartShake(const CameraShakeInfo& info);

    // 現在の揺れオフセットを取得
    // listenerPosition: 揺れを受け取る物体（カメラ）の位置。距離減衰計算に使用。
    Vector3 GetTotalOffset(const Vector3& listenerPosition) const;

    // すべての揺れを停止
    void StopAll();
	// GUI描画
    void DrawGui();

private:
    // アクティブな揺れリスト
    std::list<ShakeInstance> _activeShakes;

    // ランダム生成器
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _dist;

private:
    // デバッグ用
	float _debugAmplitude = 1.0f;
	float _debugFrequency = 20.0f;
	float _debugDuration = 0.5f;
	Vector3 _debugDirectionMask = { 1,1,1 };
	ShakeDecayType _debugDecayType = ShakeDecayType::Exponential;
};