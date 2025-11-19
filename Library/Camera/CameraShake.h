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
    Linear,     // 線形減衰（一定ペースで弱まる）
    Exponential,// 指数減衰（最初は強く、急激に弱まる。爆発など）
    None,       // 減衰なし（地震や乗り物の振動など、継続的な揺れ）
};

/// <summary>
/// シェイク発生パラメータ
/// </summary>
struct CameraShakeInfo
{
    float amplitude = 1.0f;             // 最大振幅（揺れの強さ）
    float duration = 0.5f;              // 持続時間（秒）
    float frequency = 20.0f;            // 周波数（揺れの速さ・細かさ）

    Vector3 directionMask = { 1,1,1 };  // 揺れる軸の指定（(0,1,0)なら縦揺れのみ）

    ShakeDecayType decayType = ShakeDecayType::Exponential; // 減衰タイプ

    // --- 空間音響的な設定 ---
    bool usePosition = false;           // 発生源座標を使用するか
    Vector3 sourcePosition = { 0,0,0 }; // 発生源のワールド座標
    float reachDistance = 100.0f;       // 揺れが届く限界距離
    float nearDistance = 10.0f;         // 最大強度で揺れる距離（これより近いと減衰しない）
};

/// <summary>
/// 個別の揺れインスタンス（内部管理用）
/// </summary>
struct ShakeInstance
{
    CameraShakeInfo info;
    float time = 0.0f;      // 経過時間
    Vector3 seedVector;     // ランダムシード用ベクトル
    bool isFinished = false;

    ShakeInstance(const CameraShakeInfo& info, const Vector3& seed)
        : info(info), seedVector(seed), time(0.0f) {
    }
};

/// <summary>
/// カメラシェイク管理クラス
/// </summary>
class CameraShakeManager
{
public:
    CameraShakeManager();
    ~CameraShakeManager() = default;

    // 更新処理（時間を進める）
    void Update(float elapsedTime);

    // シェイクの開始（パラメータを渡して登録）
    void StartShake(const CameraShakeInfo& info);

    // 現在の揺れオフセットを取得
    // listenerPosition: 揺れを受け取る物体（カメラ）の位置。距離減衰計算に使用。
    Vector3 GetTotalOffset(const Vector3& listenerPosition) const;

    // すべての揺れを停止
    void StopAll();

    void DrawGui();

private:
    // アクティブな揺れリスト
    std::list<ShakeInstance> _activeShakes;

    // ランダム生成器
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _dist;

    // ヘルパー：-1.0～1.0の乱数取得
    float RandomFloat();

private:
    // デバッグ用
	float _debugAmplitude = 1.0f;
	float _debugFrequency = 20.0f;
	float _debugDuration = 0.5f;
	Vector3 _debugDirectionMask = { 1,1,1 };
	ShakeDecayType _debugDecayType = ShakeDecayType::Exponential;
};