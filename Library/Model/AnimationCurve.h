#pragma once

#include <vector>
#include <algorithm>
#include <cmath>

// カーブ全体のデータを管理するクラス
class AnimationCurve
{
public:
    // キーフレームを表す構造体
    struct Keyframe
    {
        float time;  // X軸 (0.0 ~ 1.0)
        float value; // Y軸 (速度倍率)

        // 時間でソートできるように比較演算子をオーバーロード
        bool operator<(const Keyframe& other) const {
            return time < other.time;
        }
    };

public:
    // 特定の時間におけるカーブの値を取得する
    float Evaluate(float t) const
    {
        // キーがなければデフォルト値 (例: 1.0) を返す
        if (m_keyframes.empty()) {
            return 1.0f;
        }

        // 最初のキーより前なら、最初のキーの値を返す
        if (t <= m_keyframes.front().time) {
            return m_keyframes.front().value;
        }

        // 最後のキーより後なら、最後のキーの値を返す
        if (t >= m_keyframes.back().time) {
            return m_keyframes.back().value;
        }

        // 2点間の線形補間
        for (size_t i = 0; i < m_keyframes.size() - 1; ++i) {
            if (t >= m_keyframes[i].time && t <= m_keyframes[i + 1].time) {
                const Keyframe& start = m_keyframes[i];
                const Keyframe& end = m_keyframes[i + 1];

                // 2点間のtの進行度を計算
                float segmentT = (t - start.time) / (end.time - start.time);

                // 線形補間 (Lerp)
                return start.value + (end.value - start.value) * segmentT;
            }
        }

        return 1.0f; // 安全策
    }

    // キーフレームを追加し、時間順にソートする
    void AddKeyframe(float time, float value)
    {
        m_keyframes.push_back({ time, value });
        std::sort(m_keyframes.begin(), m_keyframes.end());
    }

    // 指定したインデックスのキーフレームを削除
    void RemoveKeyframe(int index) {
        if (index >= 0 && index < m_keyframes.size()) {
            m_keyframes.erase(m_keyframes.begin() + index);
        }
    }

    // ドラッグ中にキーフレームの順序が変わる可能性があるため、ソートを公開
    void SortKeyframes() {
        std::sort(m_keyframes.begin(), m_keyframes.end());
    }

    // 指定された値に最も近いキーフレームのインデックスを探す (追加時に使用)
    int FindKeyframeIndex(float time, float value) {
        for (int i = 0; i < m_keyframes.size(); ++i) {
            if (std::abs(m_keyframes[i].time - time) < 1e-6 &&
                std::abs(m_keyframes[i].value - value) < 1e-6) {
                return i;
            }
        }
        return -1;
    }

    std::vector<Keyframe>& GetKeyframes() {
        return m_keyframes;
    }

private:
    std::vector<Keyframe> m_keyframes;
};
