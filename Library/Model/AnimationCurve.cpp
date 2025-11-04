#include "AnimationCurve.h"

#include <imgui.h>
#include <imgui_internal.h>
namespace CurveEditorUI
{
    // UIの状態を管理するためのstatic変数
    static int selectedKeyframe = -1; // 選択中のキーフレームのインデックス (-1は非選択)

    // AnimationCurveオブジェクトを描画・編集する関数
    void Show(const char* label, AnimationCurve& curve)
    {
        ImGui::PushID(label);

        // --- UIの状態管理用変数を追加 ---
        static bool isDragging = false;

        ImGui::BeginChild("CurveEditor", ImVec2(0, 250), true, ImGuiWindowFlags_None);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
        const ImRect canvasRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y));

        // --- 座標変換のためのラムダ式 ---
        // カーブ座標 (0-1) -> スクリーン座標
        auto toScreen = [&](const ImVec2& p) {
            return ImVec2(
                canvasPos.x + p.x * canvasSize.x,
                canvasPos.y + (1.0f - p.y) * canvasSize.y // Y軸は上下反転
            );
            };
        // スクリーン座標 -> カーブ座標 (0-1)
        auto toCurveSpace = [&](const ImVec2& p) {
            return ImVec2(
                (p.x - canvasPos.x) / canvasSize.x,
                1.0f - (p.y - canvasPos.y) / canvasSize.y // Y軸は上下反転
            );
            };

        // 1. 背景とグリッドの描画
        drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(40, 40, 40, 255));
        // ここにグリッド線を描画するコードを追加するとより見やすくなります

        drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(180, 180, 180, 255));

        // マウスがキャンバス上にあるかチェック
        const bool isMouseOverCanvas = ImGui::IsMouseHoveringRect(canvasRect.Min, canvasRect.Max);

        auto& keyframes = curve.GetKeyframes();

        // 2. カーブの線の描画
        // (前回と同じなので省略)
        for (size_t i = 0; i < keyframes.size() - 1; ++i) {
            ImVec2 p1 = toScreen({ keyframes[i].time, keyframes[i].value });
            ImVec2 p2 = toScreen({ keyframes[i + 1].time, keyframes[i + 1].value });
            drawList->AddLine(p1, p2, IM_COL32(255, 255, 0, 255), 2.0f);
        }

        // 3. キーフレームの描画とインタラクション
        for (int i = 0; i < keyframes.size(); ++i) {
            ImVec2 pointPos = toScreen({ keyframes[i].time, keyframes[i].value });

            // マウスオーバーや選択状態で色を変える
            ImU32 pointColor = (selectedKeyframe == i) ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255);

            // 各キーフレームにインタラクション用の透明なボタンを配置
            ImGui::SetCursorScreenPos(ImVec2(pointPos.x - 5, pointPos.y - 5));
            ImGui::PushID(i);
            ImGui::InvisibleButton("##keyframe", ImVec2(10, 10));

            // --- ドラッグ処理 ---
            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
                isDragging = true;
                selectedKeyframe = i;
                ImVec2 newCurvePos = toCurveSpace(ImGui::GetIO().MousePos);

                // timeが範囲外 (0-1) に出ないようにクランプ
                keyframes[i].time = std::clamp(newCurvePos.x, 0.0f, 1.0f);
                keyframes[i].value = newCurvePos.y;

                // ドラッグで順番が入れ替わる可能性があるのでソート
                curve.SortKeyframes();
            }

            // --- 選択処理 ---
            if (ImGui::IsItemClicked()) {
                selectedKeyframe = i;
            }

            ImGui::PopID();

            drawList->AddCircleFilled(pointPos, 6.0f, pointColor);
        }

        // ドラッグが終わった瞬間にフラグをリセット
        if (isDragging && !ImGui::IsMouseDragging(0)) {
            isDragging = false;
        }

        // --- 右クリックメニューでキーフレームを追加 ---
        if (isMouseOverCanvas && ImGui::IsMouseClicked(1)) { // 1は右クリック
            ImVec2 newKeyPos = toCurveSpace(ImGui::GetIO().MousePos);
            curve.AddKeyframe(newKeyPos.x, newKeyPos.y);
            // 新しく追加したものを選択状態にする
            selectedKeyframe = curve.FindKeyframeIndex(newKeyPos.x, newKeyPos.y);
        }

        // --- Deleteキーで選択中のキーフレームを削除 ---
        if (selectedKeyframe != -1 && ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
            curve.RemoveKeyframe(selectedKeyframe);
            selectedKeyframe = -1; // 選択解除
        }

        // キャンバス外をクリックしたら選択解除
        if (ImGui::IsMouseClicked(0) && !isDragging && !ImGui::IsAnyItemActive()) {
            selectedKeyframe = -1;
        }

        ImGui::EndChild();
        ImGui::PopID();
    }
}