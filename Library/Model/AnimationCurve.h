#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

#include "../../Library/Math/Vector.h"
#include "Model.h"

// カーブ全体のデータを管理するクラス
class AnimationCurve
{
public:
    // キーフレームを表す構造体
    struct Keyframe
    {
        float time = 0.0f;  // X軸 (0.0 ~ 1.0)
        float value = 1.0f; // Y軸 (速度倍率)

        bool lockTime = false;
        bool lockValue = false;
        
        EasingType easingType = EasingType::Linear;

        // 時間でソートできるように比較演算子をオーバーロード
        bool operator<(const Keyframe& other) const {
            return time < other.time;
        }

        // シリアライズ
        template<class T>
        void serialize(T& archive, const std::uint32_t version);
    };

	// カーブデータ本体
    struct CurveData
    {
        /// <summary>
        /// 特定の時間におけるカーブの値を取得する
        /// </summary>
        /// <param name="t">特定の時間</param>
        /// <returns>カーブの値</returns>
        float Evaluate(float t) const;

        /// <summary>
        /// キーフレームを追加し、時間順にソートする
        /// </summary>
        /// <param name="time">追加する時間</param>
        /// <param name="value">値</param>
        /// <param name="lockTime">時間のロック</param>
        /// <param name="lockValue">値のロック</param>
        Keyframe* AddKeyframe(float time, float value, bool lockTime = false, bool lockValue = false);

        /// <summary>
        /// 指定したインデックスのキーフレームを削除
        /// </summary>
        /// <param name="keyPtr"></param>
        void RemoveKeyframe(Keyframe* keyPtr);

        /// <summary>
        /// ドラッグ中にキーフレームの順序が変わる可能性があるため、ソートを公開
        /// </summary>
        void SortKeyframes();

		std::string animationName;
		std::vector<Keyframe> keyframes;
		// シリアライズ
		template<class T>
		void serialize(T& archive, const std::uint32_t version);
    };

public:
	AnimationCurve() = default;
	~AnimationCurve() = default;

    /// <summary>
    /// モデル情報読み込み
    /// </summary>
    /// <param name="modelResource"></param>
    void Load(std::shared_ptr<Model> model);

    /// <summary>
    /// 特定の時間におけるカーブの値を取得する
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    /// <param name="t">特定の時間</param>
    /// <returns>カーブの値</returns>
    float Evaluate(const std::string& animationName, float t) const {
		return _data.at(animationName).Evaluate(t);
    }

    std::vector<Keyframe>& GetKeyframes(const std::string& animationName) {
		return _data[animationName].keyframes;
    }

    /// <summary>
    /// GUI描画
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
	/// <param name="currentAnimTime">現在のアニメーション経過時間</param>
	/// <param name="endAnimTime">現在のアニメーションの終了時間</param>
	/// <returns>経過時間を編集したら-1以外を返す</returns>
    float DrawGui(const std::string& animationName,
        float currentAnimTime,
        float endAnimTime);

#pragma region ファイル操作
    /// <summary>
    /// データ書き出し
    /// </summary>
    /// <param name="filename"></param>
    /// <returns>失敗したらfalse</returns>
    bool Serialize(const char* filename);

    /// <summary>
    /// データ読み込み
    /// </summary>
    /// <param name="filename"></param>
    /// <returns>失敗したらfalse</returns>
    bool Deserialize(const char* filename);
#pragma endregion
private:
    // key		: アニメーション名
    // Value	: カーブデータ 
    std::unordered_map<std::string, CurveData> _data;
};
