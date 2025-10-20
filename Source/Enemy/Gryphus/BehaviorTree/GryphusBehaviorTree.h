#pragma once

#include <memory>
#include "../../Library/Component/BehaviorController.h"
#include "../StateMachine/GryphusStateMachine.h"
#include "../../Source/AI/MetaAI.h"

class GryphusBehaviorTree : public BehaviorTree
{
public:
    GryphusBehaviorTree(
        GryphusStateMachine* stateMachine,
        Animator* animator,
        MetaAI* metaAI);
    // 開始処理
    void Start() override;
    // ビヘイビアツリー実行
    void Execute(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;
    // 現在の実行ノード名を取得
    std::string GetActiveNodeName() const { return _activeNode ? _activeNode->GetName() : ""; }

#pragma region アクセサ
    // ステートマシンを取得
    GryphusStateMachine* GetStateMachine() { return _stateMachine; }
    // アニメーターを取得
    Animator* GetAnimator() { return _animator; }
    // メタAIを取得
    MetaAI* GetMetaAI() { return _metaAI; }
#pragma endregion
private:
    // ビヘイビアツリーのGUI描画
    void DrawBehaviorTreeGui(BehaviorNodeBase<GryphusBehaviorTree>* node);
private:
    // ビヘイビアツリー
    std::unique_ptr<BehaviorTreeBase<GryphusBehaviorTree>> _behaviorTree;
    // ビヘイビアデータ
    std::unique_ptr<BehaviorData<GryphusBehaviorTree>> _behaviorData;
    // 起動中のビヘイビアノード
    BehaviorNodeBase<GryphusBehaviorTree>* _activeNode = nullptr;
    GryphusStateMachine* _stateMachine = nullptr;
    Animator* _animator = nullptr;
    MetaAI* _metaAI = nullptr;
};