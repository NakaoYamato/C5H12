#pragma once

#include "BehaviorTree.h"

#include <imgui.h>
#include <imgui_node_editor.h>
// imgui-node-editor の名前空間エイリアス
namespace ne = ax::NodeEditor;

template<class T>
class BehaviorTreeEditor
{
private:
    ne::EditorContext* _editorContext = nullptr;

    // 以前の Vertical (Y) Spacing を Horizontal (X) Spacing に使用
    const float HorizontalSpacing = 250.0f; // 階層間の水平距離
    // 新しい Vertical (Y) Spacing
    const float VerticalSpacing = 100.0f; // 兄弟ノード間の垂直距離

    //--------------------------------------------------------------------------------
    // String ハッシュ関数
    //--------------------------------------------------------------------------------
    unsigned long HashString(const std::string& str) {
        unsigned long hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + c; // hash * 33 + c
        }
        return hash;
    }

    //--------------------------------------------------------------------------------
    // SelectRule の名前を文字列に変換するヘルパー関数
    //--------------------------------------------------------------------------------
    std::string GetRuleName(SelectRule rule)
    {
        switch (rule)
        {
        case SelectRule::Priority: return "Priority";
        case SelectRule::Sequence: return "Sequence";
        case SelectRule::SequentialLooping: return "SeqLoop";
        case SelectRule::Random: return "Random";
        case SelectRule::NoDuplicatesRandom: return "NoDupRandom";
        default: return "Non";
        }
    }

    // ノードID生成関数
    // ノード名と階層番号を結合してハッシュ化し、IDの一意性を確保する
    ne::NodeId GetNodeId(BehaviorNodeBase<T>* node)
    {
        // ノード名 + 階層番号を結合
        std::string uniqueName =
            std::to_string(node->GetHirerchyNo()) + "_" + node->GetName();

        return (ne::NodeId)HashString(uniqueName);
    }

    // ピンID生成関数
    ne::PinId GetPinId(BehaviorNodeBase<T>* node, int offset)
    {
        // ノードのハッシュIDをベースにオフセットを加算
        return (ne::PinId)((size_t)GetNodeId(node) + offset);
    }

    //--------------------------------------------------------------------------------
    // 再帰的なノード描画関数 (左から右のレイアウト)
    //--------------------------------------------------------------------------------
    // 戻り値: このノードが占める垂直方向の最終Y座標
    float DrawNodeRecursive(
        BehaviorNodeBase<T>* node,
        BehaviorNodeBase<T>* activeNode,
        ImVec2 position,
        bool isInitialDraw
    )
    {
        if (node == nullptr) return position.y;

        // 1. ノードIDとピンIDの決定
        ne::NodeId nodeId = GetNodeId(node);
        ne::PinId outputPinId = GetPinId(node, 1);
        ne::PinId inputPinId = GetPinId(node, 2);

        // 2. 初期位置の設定 (初回のみ)
        // Y軸の配置は子ノードの位置に影響を与えるため、初回は固定
        if (isInitialDraw && ne::GetNodePosition(nodeId).x == FLT_MAX)
        {
            ne::SetNodePosition(nodeId, position);
        }

        // 3. ノードの描画 (色分けロジックは以前と同じ)
        // ... (色の決定ロジックは省略 - 以前のコードを参照) ...
        bool isRunning = (node == activeNode);
        bool hasAction = node->HasAction();
        bool hasJudgment = (node->GetJudgment() != nullptr);
        ImU32 nodeColor = IM_COL32(50, 50, 50, 200);

        if (hasAction) {
            nodeColor = IM_COL32(30, 100, 30, 200); // 行動ノード (緑)
        }
        else if (hasJudgment) {
            nodeColor = IM_COL32(30, 30, 100, 200); // 判定ノード (青)
        }
        if (isRunning) {
            nodeColor = IM_COL32(200, 100, 30, 200); // 実行中 (オレンジ)
        }

        ne::BeginNode(nodeId);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, nodeColor);
        if (isRunning) {
            ne::PushStyleVar(ne::StyleVar_NodeBorderWidth, 3.0f);
        }

        // --- 入力ピン (左側に配置) ---
        if (node->GetParent() != nullptr)
        {
            ne::BeginPin(inputPinId, ne::PinKind::Input);
            ImGui::Dummy(ImVec2(10, 10));
            ne::EndPin();
            ImGui::SameLine();
        }

        // ノードの主要コンテンツ
        ImGui::BeginGroup();
        ImGui::Text("%d: %s", node->GetHirerchyNo(), node->GetName().c_str());
        std::string ruleName = GetRuleName(node->GetSelectRule());
        ImGui::TextDisabled("Rule: %s", ruleName.c_str());
        if (hasAction) {
            ImGui::TextDisabled("Action / Priority: %d", node->GetPriority());
        }
        if (hasJudgment) {
            ImGui::TextDisabled("Has Judgment");
        }
        ImGui::EndGroup();

        // --- 出力ピン (右側に配置) ---
        if (node->GetChild(0) != nullptr)
        {
            ImGui::SameLine();
            ne::BeginPin(outputPinId, ne::PinKind::Output);
            ImGui::Dummy(ImVec2(10, 10));
            ne::EndPin();
        }

        if (isRunning) {
            ne::PopStyleVar(1);
        }
        ImGui::PopStyleColor(1);
        ne::EndNode();

        // 4. 子ノードの描画とリンクの作成
        if (node->GetChild(0) != nullptr)
        {
            // ノードのサイズとピンの配置に必要なスペースを取得
            ImVec2 nodeSize = ne::GetNodeSize(nodeId);

            // 子ノードの描画開始位置
            float nextX = position.x + nodeSize.x + HorizontalSpacing;
            float currentY = position.y;

            // 子ノードの総垂直スペースを計算 (中央揃えのため)
            float childrenCount = (float)node->GetChildrenSize();
            float totalChildrenHeight = childrenCount * VerticalSpacing;
            float startY = currentY - (totalChildrenHeight / 2.0f) + (VerticalSpacing / 2.0f);

            float lastChildY = startY;

            // 子ノードを垂直に並べる (Y座標を変更)
            for (size_t i = 0; i < childrenCount; ++i)
            {
                BehaviorNodeBase<T>* child = node->GetChild((int)i).get();

                // 子ノードの位置を計算
                ImVec2 childPosition = ImVec2(nextX, startY + i * VerticalSpacing);

                // 子ノードの入力ピンIDを決定
                ne::PinId childInputPinId = GetPinId(child, 2);

                // 子ノードを再帰的に描画
                lastChildY = DrawNodeRecursive(child, activeNode, childPosition, isInitialDraw);

                // リンクの描画 (親の出力ピン -> 子の入力ピン)
                ne::Link((ne::LinkId)((size_t)outputPinId + (size_t)i), outputPinId, childInputPinId);
            }

            // このノードが占める垂直方向の最終Y座標を返す
            return lastChildY;
        }

        // 子ノードがない場合、現在のノードのY座標を返す
        return position.y;
    }

public:
    // コンストラクタ
    BehaviorTreeEditor()
    {
        ne::Config config;
        config.SettingsFile = "Test.json"; // 状態保存ファイル名
        _editorContext = ne::CreateEditor(&config);
    }

    // デストラクタ
    ~BehaviorTreeEditor()
    {
        if (_editorContext)
        {
            ne::DestroyEditor(_editorContext);
        }
    }

    //--------------------------------------------------------------------------------
    // メインの描画関数
    //--------------------------------------------------------------------------------
    void Draw(BehaviorTreeBase<T>* tree, BehaviorNodeBase<T>* activeNode)
    {
        if (tree == nullptr || _editorContext == nullptr) return;

        ne::SetCurrentEditor(_editorContext);

        ImGui::Begin("Behavior Tree Editor", nullptr, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Fit View"))
                {
                    ne::NavigateToContent();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ne::Begin("BehaviorTreeSpace");

        //ノード位置の初回設定フラグを決定
        ne::NodeId rootId = GetNodeId(tree->GetRoot().get());

        bool isInitialDraw = (ne::GetNodePosition(rootId).x == FLT_MAX);

        // ルートノードの初期位置を設定 (画面左端、垂直方向の中央付近)
        ImVec2 rootPosition = ImVec2(50.0f, 300.0f);

        // 初回描画フラグを true に設定
        DrawNodeRecursive(tree->GetRoot().get(), activeNode, rootPosition, isInitialDraw);

        ne::End();
        ImGui::End();
    }
};