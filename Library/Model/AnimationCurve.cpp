#include "AnimationCurve.h"

#include <filesystem>
#include <fstream>

#include <imgui.h>
#include <imgui_internal.h>

#include "SerializeFunction.h"

#define ANIM_CURVE_VERSION_0 0

// データのバージョン管理
CEREAL_CLASS_VERSION(AnimationCurve, ANIM_CURVE_VERSION_0)

namespace CurveEditorUI
{
	// 選択中のキーフレーム
    static AnimationCurve::Keyframe* selectedKeyframePtr = nullptr;
    // ドラッグ状態を保持する静的変数
    static bool isDragging = false;
    // Y軸の表示範囲
    constexpr float minYValue = 0.0f;
    constexpr float maxYValue = 2.0f;
    constexpr float valueRange = maxYValue - minYValue; // = 2.0f
    const int steps = 100; // 描画の細かさ (キャンバス幅に応じて調整可)
    // グリッドの垂直線分割数
	constexpr int verticalGridLines = 4;
	// グリッドの水平線分割数
	constexpr int horizontalGridLines = 4;

    // AnimationCurveオブジェクトを描画・編集する関数
    static bool Show(const char* label, AnimationCurve::CurveData& curve, float& normalizedTime)
    {
        ImGui::PushID(label);

        ImGui::BeginChild("CurveEditor", ImVec2(0, 250), true, ImGuiWindowFlags_None);

        // 左側に目盛り表示用のマージンを確保
        const float leftMargin = 40.0f;
        const ImVec2 windowPos = ImGui::GetCursorScreenPos(); // Child Window の左上隅

        // canvasの開始位置とサイズをマージン分調整
        const ImVec2 canvasPos = ImVec2(windowPos.x + leftMargin, windowPos.y);
        const ImVec2 availableSize = ImGui::GetContentRegionAvail();
        const ImVec2 canvasSize = ImVec2(std::max<float>(0.0f, availableSize.x - leftMargin), availableSize.y -0.01f);

        const ImRect canvasRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y));
        const bool isMouseOverCanvas = ImGui::IsMouseHoveringRect(canvasRect.Min, canvasRect.Max);

        const ImU32 gridColor = IM_COL32(60, 60, 60, 255);
        const ImU32 textColor = IM_COL32(200, 200, 200, 255);

        // 戻り値用のフラグ
        bool timeWasChangedByUI = false;

        // 座標変換
        auto toScreen = [&](const ImVec2& p) {
            // 値を0.0-1.0の範囲に正規化
            float normalizedY = (p.y - minYValue) / valueRange;
            return ImVec2(
                canvasPos.x + p.x * canvasSize.x,
                canvasPos.y + (1.0f - normalizedY) * canvasSize.y // Yは反転
            );
            };
        auto toCurveSpace = [&](const ImVec2& p) {
            // スクリーンY座標を0.0-1.0の範囲に正規化
            float normalizedY = 1.0f - (p.y - canvasPos.y) / canvasSize.y;
            return ImVec2(
                // Time:0.0-1.0の範囲にクランプ
                std::clamp((p.x - canvasPos.x) / canvasSize.x, 0.0f, 1.0f),
                // Value: minYValue-maxYValueの範囲にクランプ
                std::clamp(minYValue + normalizedY * valueRange, minYValue, maxYValue)
            );
            };

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        
        // 背景描画 (マージン領域も含む)
        drawList->AddRectFilled(windowPos, ImVec2(windowPos.x + availableSize.x, windowPos.y + availableSize.y), IM_COL32(30, 30, 30, 255));
        // キャンバス領域の背景
        drawList->AddRectFilled(canvasPos, canvasRect.Max, IM_COL32(40, 40, 40, 255));

        // グリッドと目盛り数値の描画
        // 垂直線
        for (int i = 1; i < verticalGridLines; ++i) 
        {
            float t = (float)i / verticalGridLines;
            float x = toScreen({ t, 0.0f }).x;
            drawList->AddLine(ImVec2(x, canvasPos.y), ImVec2(x, canvasRect.Max.y), gridColor);
        }
        // 水平線
        for (int i = 0; i <= horizontalGridLines; ++i)
        {
			float v = minYValue + i * (valueRange / horizontalGridLines);
            float y = toScreen({ 0.0f, v }).y;
            // 線の色 (1.0 の中心線)
            bool isCenterLine = (std::abs(v - 1.0f) < 1e-6);
            ImU32 lineColor = isCenterLine ? IM_COL32(110, 110, 110, 255) : gridColor;
            float lineThickness = isCenterLine ? 2.0f : 1.0f;

            drawList->AddLine(ImVec2(canvasPos.x, y), ImVec2(canvasRect.Max.x, y), lineColor, lineThickness);

            // 目盛り数値の描画
            char text[16];
            snprintf(text, 16, "%.1f", v);
            ImVec2 textSize = ImGui::CalcTextSize(text);

            // テキストをY軸中心に、マージンの右端に揃える
            ImVec2 textPos = ImVec2(
                canvasPos.x - textSize.x - 4.0f, // 4pxのパディング
                y - textSize.y * 0.5f
            );
            drawList->AddText(textPos, textColor, text);
        }

        // 枠線
        drawList->AddRect(canvasPos, canvasRect.Max, IM_COL32(180, 180, 180, 255));

        // 再生時間のバーを描画
        const ImU32 barColor = IM_COL32(255, 80, 80, 200); // 目立つ色 (赤)
        float barScreenX = toScreen({ normalizedTime, 0.0f }).x;

        // バー本体の線
        drawList->AddLine(ImVec2(barScreenX, canvasPos.y), ImVec2(barScreenX, canvasRect.Max.y), barColor, 2.0f);

        // バーを操作するための透明なボタンを配置 (幅8px)
        ImGui::SetCursorScreenPos(ImVec2(barScreenX - 4, canvasPos.y));
        ImGui::InvisibleButton("##playback_bar", ImVec2(8, canvasSize.y));

        // ホバー時にマウスカーソルを変更
        if (ImGui::IsItemHovered()) 
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); // 左右矢印
        }

        // ドラッグ操作
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
        {
            ImVec2 newCurvePos = toCurveSpace(ImGui::GetIO().MousePos);
            normalizedTime = newCurvePos.x; // 参照を直接更新
            timeWasChangedByUI = true;      // 変更フラグを立てる
            isDragging = true;              // キーの選択解除を防ぐ
        }

        auto& keyframes = curve.keyframes;

        drawList->PushClipRect(canvasRect.Min, canvasRect.Max, true);
        // カーブの線の描画
        // Evaluate を細かく呼び出して曲線を描画する
        if (keyframes.size() > 0)
        {
            ImVec2 p1 = toScreen({ 0.0f, curve.Evaluate(0.0f) }); // 開始点

            for (int i = 1; i <= steps; ++i) 
            {
                float t = (float)i / (float)steps;
                float v = curve.Evaluate(t);
                ImVec2 p2 = toScreen({ t, v });
                drawList->AddLine(p1, p2, IM_COL32(255, 255, 0, 255), 2.0f);
                p1 = p2;
            }
        }
        drawList->PopClipRect();

        // キーフレームの描画とインタラクション
        for (int i = 0; i < keyframes.size(); ++i) 
        {
            ImVec2 pointPos = toScreen({ keyframes[i].time, keyframes[i].value });

            // 選択状態で色を変更
            bool isSelected = (selectedKeyframePtr == &keyframes[i]);
            ImU32 pointColor = isSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255);
            // ボタンの半径 (円の半径や四角形の半分のサイズと合わせる)
            const float buttonRadius = 6.0f;

            // 理想的なボタンの境界
            ImVec2 buttonMin(pointPos.x - buttonRadius, pointPos.y - buttonRadius);
            ImVec2 buttonMax(pointPos.x + buttonRadius, pointPos.y + buttonRadius);

            // キャンバスの境界 (canvasRect) でクリップする
            buttonMin.x = std::max<float>(buttonMin.x, canvasRect.Min.x);
            buttonMin.y = std::max<float>(buttonMin.y, canvasRect.Min.y);
            buttonMax.x = std::min<float>(buttonMax.x, canvasRect.Max.x);
            buttonMax.y = std::min<float>(buttonMax.y, canvasRect.Max.y);

            // クリップされた座標とサイズでボタンを配置
            ImGui::SetCursorScreenPos(buttonMin);
            ImGui::PushID(i);
            ImGui::InvisibleButton("##keyframe", ImVec2(buttonMax.x - buttonMin.x, buttonMax.y - buttonMin.y));

            // ドラッグ処理
            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) 
            {
                isDragging = true;
                selectedKeyframePtr = &keyframes[i];
                // toCurveSpaceは既にクランプ済みの値を返す
                ImVec2 newCurvePos = toCurveSpace(ImGui::GetIO().MousePos);

                bool timeChanged = false;
                if (!keyframes[i].lockTime) {
                    keyframes[i].time = newCurvePos.x; // 0.0-1.0
                    timeChanged = true;
                }
                if (!keyframes[i].lockValue) {
                    keyframes[i].value = newCurvePos.y; // 0.0-2.0
                }

                if (timeChanged) {
                    curve.SortKeyframes();
                }
            }

            if (ImGui::IsItemClicked()) {
                selectedKeyframePtr = &keyframes[i];
            }
            ImGui::PopID();

            // 描画 (ロックされている場合は四角形)
            if (keyframes[i].lockTime || keyframes[i].lockValue) 
            {
                drawList->AddRectFilled(ImVec2(pointPos.x - 5, pointPos.y - 5), ImVec2(pointPos.x + 5, pointPos.y + 5), pointColor);
            }
            else 
            {
                drawList->AddCircleFilled(pointPos, 6.0f, pointColor);
            }
        }

        // ドラッグが終わった瞬間にフラグをリセット
        if (isDragging && !ImGui::IsMouseDragging(0))
        {
            isDragging = false;
        }

        // 右クリックで追加 (ポインタで選択)
        if (isMouseOverCanvas && ImGui::IsMouseClicked(1)) {
            ImVec2 newKeyPos = toCurveSpace(ImGui::GetIO().MousePos);
            AnimationCurve::Keyframe* newKey = curve.AddKeyframe(newKeyPos.x, newKeyPos.y);
            selectedKeyframePtr = newKey;
        }

        // キャンバス外クリックで選択解除
        if (ImGui::IsMouseClicked(0) && !isDragging && !ImGui::IsAnyItemHovered()) {
            selectedKeyframePtr = nullptr;
        }

        ImGui::EndChild();

        // インスペクターGUI
        ImGui::Separator();
        if (selectedKeyframePtr)
        {
            ImGui::Text("Selected Keyframe Properties");

            // ImGuiのUIIDが "Time" などで重複しないよう、キーのポインタを使ってユニークなIDをPush
            ImGui::PushID(selectedKeyframePtr);

            ImGui::Checkbox("Lock Time", &selectedKeyframePtr->lockTime);
            ImGui::SameLine();
			bool timeEdited = false;
            if (selectedKeyframePtr->lockTime)
                ImGui::Text("Time: %.3f", selectedKeyframePtr->time);
            else
                timeEdited = ImGui::DragFloat("Time", &selectedKeyframePtr->time, 0.001f, 0.0f, 1.0f, "%.3f");

            ImGui::Checkbox("Lock Value", &selectedKeyframePtr->lockValue);
            ImGui::SameLine();
            if (selectedKeyframePtr->lockValue)
                ImGui::Text("Value: %.3f", selectedKeyframePtr->value);
            else
                ImGui::DragFloat("Value", &selectedKeyframePtr->value, 0.01f, minYValue, maxYValue, "%.3f");

            // 2番目以降のキーフレームのイージングタイプを編集
            if (selectedKeyframePtr != &keyframes[0])
            {
                Easings::DrawGui(selectedKeyframePtr->easingType);
            }

            // インスペクターでTimeが編集されたらソートを実行
            if (timeEdited && ImGui::IsItemDeactivatedAfterEdit()) 
            {
                curve.SortKeyframes();
            }

			if (ImGui::Button("Delete Keyframe"))
			{
				curve.RemoveKeyframe(selectedKeyframePtr);
				selectedKeyframePtr = nullptr;
			}

            ImGui::PopID();
        }
        else
        {
            ImGui::Text("No keyframe selected.");
        }

        ImGui::PopID(); 
        
        return timeWasChangedByUI;
    }
}

template<class T>
inline void AnimationCurve::Keyframe::serialize(T& archive, 
    const std::uint32_t version)
{
    if (version == ANIM_CURVE_VERSION_0)
    {
        archive(
            CEREAL_NVP(this->time),
            CEREAL_NVP(this->value),
			CEREAL_NVP(this->lockTime),
			CEREAL_NVP(this->lockValue),
			CEREAL_NVP(this->easingType)
        );
    }
}

/// 特定の時間におけるカーブの値を取得する
float AnimationCurve::CurveData::Evaluate(float t) const
{
    // 境界チェック
    if (keyframes.empty()) {
        return 1.0f; // デフォルト値 (Y=1.0)
    }
    if (t <= keyframes.front().time) {
        return keyframes.front().value;
    }
    if (t >= keyframes.back().time) {
        return keyframes.back().value;
    }

    // 該当するキーフレームセグメントを探す
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        const Keyframe& start = keyframes[i];
        const Keyframe& end = keyframes[i + 1];

        if (t >= start.time && t <= end.time) {
            // セグメント内のローカルな t (0.0 - 1.0) を計算
            float segmentDuration = end.time - start.time;
            if (segmentDuration <= 1e-6f) { // ゼロ除算防止
                return end.value;
            }
            float segmentT = (t - start.time) / segmentDuration;

            // 3. EasingLerp を使って補間する
            // (補間方法は「終了点(end)」のキーフレーム設定に従う仕様)
            return EasingLerp(start.value, end.value, segmentT, end.easingType);
        }
    }

    return 1.0f; // 安全策
}

/// キーフレームを追加し、時間順にソートする
AnimationCurve::Keyframe* AnimationCurve::CurveData::AddKeyframe(float time, float value, bool lockTime, bool lockValue)
{
    keyframes.push_back({ time, value, lockTime, lockValue });
    SortKeyframes();

    // ソート後に追加したキーを探してポインタを返す
    for (auto& key : keyframes) {
        if (std::abs(key.time - time) < 1e-6 && std::abs(key.value - value) < 1e-6) {
            return &key;
        }
    }
    return nullptr;
}

/// 指定したインデックスのキーフレームを削除
void AnimationCurve::CurveData::RemoveKeyframe(Keyframe* keyPtr)
{
    if (!keyPtr) return;

    for (auto it = keyframes.begin(); it != keyframes.end(); ++it) {
        // ポインタが一致するかどうかで削除対象を検索
        if (&(*it) == keyPtr) {
            keyframes.erase(it);
            return;
        }
    }
}

/// ドラッグ中にキーフレームの順序が変わる可能性があるため、ソートを公開
void AnimationCurve::CurveData::SortKeyframes()
{
    std::sort(keyframes.begin(), keyframes.end());
}

template<class T>
inline void AnimationCurve::CurveData::serialize(T& archive, const std::uint32_t version)
{
    if (version == ANIM_CURVE_VERSION_0)
    {
        archive(
            CEREAL_NVP(this->animationName),
            CEREAL_NVP(this->keyframes)
        );
    }
}

/// モデル情報読み込み
void AnimationCurve::Load(std::shared_ptr<Model> model)
{
    assert(model != nullptr);

    // モデルのシリアライズパスからイベント情報のパスを取得
    if (!Deserialize(model->GetFilename()))
    {
        // なかったら新規作成
        // アニメーション名を登録
        for (auto& animation : model->GetResource()->GetAnimations())
        {
            CurveData tempCurve;
			tempCurve.animationName = animation.name;
            tempCurve.AddKeyframe(0.0f, 1.0f, true, false);
			tempCurve.AddKeyframe(1.0f, 1.0f, true, false);

			_data[animation.name] = tempCurve;
        }
    }
}

/// GUI描画
float AnimationCurve::DrawGui(const std::string& animationName,
    float currentAnimTime,
    float endAnimTime)
{
	float currentTimeNormalized = currentAnimTime / endAnimTime;
    if (CurveEditorUI::Show(animationName.c_str(),
        this->_data[animationName],
        currentTimeNormalized))
    {
		return currentTimeNormalized * endAnimTime;
    }
	return -1.0f;
}

#pragma region ファイル操作
/// データ書き出し
bool AnimationCurve::Serialize(const char* filename)
{
    std::filesystem::path serializePath(filename);
    serializePath.replace_extension(ANIMATION_CURVE_EXTENSION);

    std::ofstream ostream(serializePath.string().c_str(), std::ios::binary);
    if (ostream.is_open())
    {
        cereal::BinaryOutputArchive archive(ostream);

        try
        {
            archive(
                CEREAL_NVP(_data)
            );
            return true;
        }
        catch (...)
        {
        }
    }
    return false;
}

/// データ読み込み
bool AnimationCurve::Deserialize(const char* filename)
{
    std::filesystem::path serializePath(filename);
    serializePath.replace_extension(ANIMATION_CURVE_EXTENSION);

    std::ifstream istream(serializePath.string().c_str(), std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {


            archive(
                CEREAL_NVP(_data)
            );
            return true;
        }
        catch (...)
        {
        }
    }
    return false;
}
#pragma endregion
