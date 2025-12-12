#pragma once

#include "BehaviorTree.h"

#include "../../Library/Exporter/Exporter.h"

#include <unordered_map>
#include <imgui.h>
#include <imgui_node_editor.h>
// imgui-node-editor の名前空間エイリアス
namespace ne = ax::NodeEditor;

template<class T>
class BehaviorTreeEditor
{
private:
    ne::EditorContext* _editorContext = nullptr;

    std::string _filename = "";
    std::unordered_map<size_t, ImVec2> _nodePositions;

    // 以前の Vertical (Y) Spacing を Horizontal (X) Spacing に使用
    const float HorizontalSpacing = 250.0f; // 階層間の水平距離
    // 新しい Vertical (Y) Spacing
    const float VerticalSpacing = 100.0f; // 兄弟ノード間の垂直距離

    // デフォルト色(複合ノード)
	ImVec4 _defaultNodeColor = ImVec4(50 / 255.0f, 50 / 255.0f, 50 / 255.0f, 200 / 255.0f);
    // デフォルト枠線色
	ImVec4 _borderColor = ImVec4(100 / 255.0f, 100 / 255.0f, 100 / 255.0f, 255 / 255.0f);
    // 実行中色
	ImVec4 _activeNodeColor = ImVec4(200 / 255.0f, 100 / 255.0f, 30 / 255.0f, 200 / 255.0f);
	// 実行中ノード枠線色
	ImVec4 _activeNodeBorderColor = ImVec4(255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 255 / 255.0f);
    // 行動ノード色
	ImVec4 _actionNodeColor = ImVec4(30 / 255.0f, 100 / 255.0f, 30 / 255.0f, 200 / 255.0f);
    // 判定ノード色
	ImVec4 _judgmentNodeColor = ImVec4(30 / 255.0f, 30 / 255.0f, 100 / 255.0f, 200 / 255.0f);

    // String ハッシュ関数
    unsigned long HashString(const std::string& str) {
        unsigned long hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + c; // hash * 33 + c
        }
        return hash;
    }

    // SelectRule の名前を文字列に変換するヘルパー関数
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

    // 再帰的なノード描画関数
    // 戻り値: このノードが占める垂直方向の最終Y座標
    float DrawNodeRecursive(
        BehaviorNodeBase<T>* node,
        BehaviorNodeBase<T>* activeNode,
        ImVec2 position
    )
    {
        if (node == nullptr) return position.y;

        // ノードIDとピンIDの決定
        ne::NodeId nodeId = GetNodeId(node);
        ne::PinId outputPinId = GetPinId(node, 1);
        ne::PinId inputPinId = GetPinId(node, 2);

        // 初期位置の設定 (初回のみ)
        if (ne::GetNodePosition(nodeId).x == FLT_MAX)
        {
            if (_nodePositions.find((size_t)nodeId) == _nodePositions.end())
            {
                // 初回描画時の位置を設定
                _nodePositions[(size_t)nodeId] = position;
            }
            ne::SetNodePosition(nodeId, _nodePositions[(size_t)nodeId]);
        }
        else
        {
            // 位置情報を保存
            _nodePositions[(size_t)nodeId] = ne::GetNodePosition(nodeId);
        }

        // ノードの描画
        bool isRunning = (node == activeNode);
        bool hasAction = node->HasAction();
        bool hasJudgment = (node->GetJudgment() != nullptr);

        ImVec4 bgColor = _defaultNodeColor;
        ImVec4 borderColor = _borderColor;

        if (hasAction)
            bgColor = _actionNodeColor;
        else if (hasJudgment)
            bgColor = _judgmentNodeColor;

        // 実行中のノードはハイライト (最優先)
        if (isRunning)
        {
            bgColor = _activeNodeColor;
            borderColor = _activeNodeBorderColor;
        }
        ne::PushStyleColor(ne::StyleColor_NodeBg, bgColor);
        ne::PushStyleColor(ne::StyleColor_NodeBorder, borderColor);
        // 実行中のノードは枠線を太くして強調
        if (isRunning) 
        {
            ne::PushStyleVar(ne::StyleVar_NodeBorderWidth, 3.0f);
        }
        ne::BeginNode(nodeId);

        // 入力ピン (左側に配置)
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
        if (hasAction)
        {
            ImGui::TextDisabled("Action / Priority: %d", node->GetPriority());
        }
        if (hasJudgment)
        {
            ImGui::TextDisabled("Has Judgment");
        }
        ImGui::EndGroup();

        // 出力ピン (右側に配置)
        if (node->GetChild(0) != nullptr)
        {
            ImGui::SameLine();
            ne::BeginPin(outputPinId, ne::PinKind::Output);
            ImGui::Dummy(ImVec2(10, 10));
            ne::EndPin();
        }

        ne::EndNode();

        if (isRunning)
        {
            ne::PopStyleVar(1);
        }
        ne::PopStyleColor(2); // NodeBg と NodeBorder を Pop

        // 子ノードの描画とリンクの作成
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
                lastChildY = DrawNodeRecursive(child, activeNode, childPosition);

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
    BehaviorTreeEditor()
    {
        ne::Config config;
        config.SettingsFile = ""; // imgui側で保存しない
        _editorContext = ne::CreateEditor(&config);
    }

    ~BehaviorTreeEditor()
    {
        if (_editorContext)
        {
            ne::DestroyEditor(_editorContext);
        }

        SaveToFile();
    }

    // 描画関数
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

        // ルートノードの初期位置を設定 (画面左端、垂直方向の中央付近)
        ImVec2 rootPosition = ImVec2(50.0f, 300.0f);

        // 初回描画フラグを true に設定
        DrawNodeRecursive(tree->GetRoot().get(), activeNode, rootPosition);

        ne::End();
        ImGui::End();
    }

    // ファイル読み込み
    void LoadFromFile(const std::string& filename)
    {
        _filename = filename;

        nlohmann::json jsonData;
        if (Exporter::LoadJsonFile(_filename, &jsonData))
        {
            size_t size = jsonData["Size"].get<std::size_t>();
            for (size_t i = 0; i < size; ++i)
            {
                size_t nodeId = jsonData["NodeId_" + std::to_string(i)].get<std::size_t>();
                float posX = jsonData["NodePosX_" + std::to_string(i)].get<float>();
                float posY = jsonData["NodePosY_" + std::to_string(i)].get<float>();
                _nodePositions[nodeId] = ImVec2(posX, posY);
                ne::SetNodePosition(nodeId, ImVec2(posX, posY));
            }
        }
    }

    // ファイル保存
    void SaveToFile()
    {
        if (_filename.empty()) return;

        nlohmann::json jsonData;
        jsonData["Size"] = _nodePositions.size();
        size_t index = 0;
        for (auto& [nodeId, pos] : _nodePositions)
        {
            jsonData["NodeId_" + std::to_string(index)] = nodeId;
            jsonData["NodePosX_" + std::to_string(index)] = pos.x;
            jsonData["NodePosY_" + std::to_string(index)] = pos.y;
            ++index;
        }
        // ファイルのシリアライズ処理
        Exporter::SaveJsonFile(_filename, jsonData);
    }

};