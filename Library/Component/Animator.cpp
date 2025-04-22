#include "Animator.h"

#include "../../Library/JobSystem/JobSystem.h"

#include <imgui.h>

Animator::Animator(Model* model) :
	_model(model)
{
	// ノードキャッシュの生成
	_nodeCaches.resize(model->GetPoseNodes().size());
}

// 更新処理
void Animator::Update(float elapsedTime)
{

    //std::vector<std::future<void>> jobResults;
    //// ジョブシステムに計算を任せる
    //jobResults.emplace_back(JobSystem::Instance().EnqueueJob("Animation",
    //    ImGuiControl::Profiler::Color::Green,
    //    [&]()
    //    {
    //        UpdateAnimation(elapsedTime);
    //    }
    //));

    //// すべてのジョブの終了を待機
    //for (auto& result : jobResults)
    //{
    //    result.get();
    //}

    //JobSystem::Instance().jobResults.emplace_back(JobSystem::Instance().EnqueueJob("Animation",
    //    ImGuiControl::Profiler::Color::Green,
    //    [&]()
    //    {
    //        UpdateAnimation(elapsedTime);
    //    }
    //));
    UpdateAnimation(elapsedTime);
}

// GUI描画
void Animator::DrawGui()
{
    if (ImGui::TreeNode(u8"アニメーション"))
    {
        if (_currentAnimIndex >= 0)
        {
            ImGui::Text(u8"再生中のアニメーション:");
            ImGui::SameLine();
            ImGui::Text(_model->GetResource()->GetAnimations()[_currentAnimIndex].name.c_str());

            ImGui::Separator();
        }
        ImGui::DragFloat("CurrentAnimSeconds", &_currentAnimSeconds, 0.01f);
        ImGui::DragFloat("BlendSeconds", &_animBlendSeconds, 0.01f);
        ImGui::Checkbox("Loop", &_animLoop);

        int index = 0;
        for (const ModelResource::Animation& animation : _model->GetResource()->GetAnimations())
        {
            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

            if (ImGui::TreeNodeEx(&animation, nodeFlags, animation.name.c_str()))
            {
                // ダブルクリックで再生
                if (ImGui::IsItemClicked())
                {
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        this->PlayAnimation(index, _animLoop, _animBlendSeconds);
                    }
                }

                ImGui::TreePop();
            }

            index++;
        }

        ImGui::TreePop();
    }
}

// アニメーション更新処理
void Animator::UpdateAnimation(float elapsedTime)
{
    if (_currentAnimIndex == -1)
        return;
    std::vector<ModelResource::Node>& poseNode = _model->GetPoseNodes();
    // アニメーション計算処理
    ComputeAnimation(_currentAnimIndex, _currentAnimSeconds, poseNode);
    // ノード設定
    //_model->SetPoseNodes(poseNode);
    // アニメーション経過時間更新
    UpdateAnimSeconds(elapsedTime);

    // ブレンディング計算処理
    if (_animBlending)
    {
        // ブレンド率計算
        float rate = _currentAnimBlendSeconds / _animBlendSecondsLength;

        poseNode = ComputeBlending(_nodeCaches, _model->GetPoseNodes(), rate);
        // ノード設定
        _model->SetPoseNodes(poseNode);

        // 時間経過
        _currentAnimBlendSeconds += elapsedTime;
        if (_currentAnimBlendSeconds >= _animBlendSecondsLength)
        {
            _currentAnimBlendSeconds = _animBlendSecondsLength;
            _animBlending = false;
        }
    }
}

// アニメーション経過時間更新
void Animator::UpdateAnimSeconds(float elapsedTime)
{
    // 経過時間
    _currentAnimSeconds += elapsedTime;
    // 再生時間が終端時間を超えた時
    const ModelResource::Animation& animation = _model->GetResource()->GetAnimations().at(_currentAnimIndex);
    if (_currentAnimSeconds > animation.secondsLength)
    {
        if (_animLoop)
        {
            // 再生時間を戻す
            _currentAnimSeconds -= animation.secondsLength;
        }
        else
        {
            // 再生時間を終了時間にする
            _currentAnimSeconds = animation.secondsLength;
            _animPlaying = false;
        }
    }
}

// アニメーション再生
void Animator::PlayAnimation(int index, bool loop, float blendSeconds)
{
    _currentAnimIndex = index;
    _currentAnimSeconds = 0;
    _animLoop = loop;
    _animPlaying = true;

    // ブレンドアニメーションパラメーター
    _animBlending = blendSeconds > 0.0f;
    _currentAnimBlendSeconds = 0.0f;
    _animBlendSecondsLength = blendSeconds;

    // 現在の姿勢をキャッシュする
    for (size_t i = 0; i < _model->GetPoseNodes().size(); ++i)
    {
        const ModelResource::Node& src = _model->GetPoseNodes().at(i);
        ModelResource::Node& dst = _nodeCaches.at(i);

        dst.position = src.position;
        dst.rotation = src.rotation;
        dst.scale = src.scale;
    }
}

// アニメーション再生(名前から検索)
void Animator::PlayAnimation(std::string name, bool loop, float blendSeconds)
{
    PlayAnimation(GetAnimationIndex(name), loop, blendSeconds);
}

// アニメーション再生中か
bool Animator::IsPlayAnimation(int index) const
{
    if (index != -1)
    {
        if (_currentAnimIndex != index)return false;
    }
    if (_currentAnimIndex < 0)return false;
    if (_currentAnimIndex >= _model->GetResource()->GetAnimations().size()) return false;
    return _animPlaying;
}

// アニメーション計算処理
void Animator::ComputeAnimation(int animationIndex, int nodeIndex, float time, ModelResource::Node& nodePose) const
{
    // 指定のアニメーションデータを収集
    const ModelResource::Animation& animation = _model->GetResource()->GetAnimations().at(animationIndex);
    const ModelResource::NodeAnim& nodeAnim = animation.nodeAnims.at(nodeIndex);
    // 位置
    for (size_t index = 0; index < nodeAnim.positionKeyframes.size() - 1; ++index)
    {
        // 現在の時間がどのキーフレームの間か判定
        const ModelResource::VectorKeyframe& key0 = nodeAnim.positionKeyframes.at(index);
        const ModelResource::VectorKeyframe& key1 = nodeAnim.positionKeyframes.at(index + 1);
        if (time >= key0.seconds && time < key1.seconds)
        {
            // 再生時間とキーフレームの時間から補完率を計算
            float rate = (time - key0.seconds) /
                (key1.seconds - key0.seconds);

            // 前のキーフレームと次のキーフレームの姿勢を補完
            DirectX::XMVECTOR V0 = DirectX::XMLoadFloat3(&key0.value);
            DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&key1.value);
            // ノードに格納
            DirectX::XMStoreFloat3(&nodePose.position, DirectX::XMVectorLerp(V0, V1, rate));
        }
    }
    // 回転
    for (size_t index = 0; index < nodeAnim.rotationKeyframes.size() - 1; ++index)
    {
        // 現在の時間がどのキーフレームの間か判定
        const ModelResource::QuaternionKeyframe& key0 = nodeAnim.rotationKeyframes.at(index);
        const ModelResource::QuaternionKeyframe& key1 = nodeAnim.rotationKeyframes.at(index + 1);
        if (time >= key0.seconds && time < key1.seconds)
        {
            // 再生時間とキーフレームの時間から補完率を計算
            float rate = (time - key0.seconds) /
                (key1.seconds - key0.seconds);

            // 前のキーフレームと次のキーフレームの姿勢を補完
            DirectX::XMVECTOR Q0 = DirectX::XMLoadFloat4(&key0.value);
            DirectX::XMVECTOR Q1 = DirectX::XMLoadFloat4(&key1.value);
            // ノードに格納
            DirectX::XMStoreFloat4(&nodePose.rotation, DirectX::XMQuaternionSlerp(Q0, Q1, rate));
        }
    }
    // スケール
    for (size_t index = 0; index < nodeAnim.scaleKeyframes.size() - 1; ++index)
    {
        // 現在の時間がどのキーフレームの間か判定
        const ModelResource::VectorKeyframe& key0 = nodeAnim.scaleKeyframes.at(index);
        const ModelResource::VectorKeyframe& key1 = nodeAnim.scaleKeyframes.at(index + 1);
        if (time >= key0.seconds && time < key1.seconds)
        {
            // 再生時間とキーフレームの時間から補完率を計算
            float rate = (time - key0.seconds) /
                (key1.seconds - key0.seconds);

            // 前のキーフレームと次のキーフレームの姿勢を補完
            DirectX::XMVECTOR V0 = DirectX::XMLoadFloat3(&key0.value);
            DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&key1.value);
            // ノードに格納
            DirectX::XMStoreFloat3(&nodePose.scale, DirectX::XMVectorLerp(V0, V1, rate));
        }
    }
}

// アニメーション計算処理
void Animator::ComputeAnimation(int animationIndex, float time, std::vector<ModelResource::Node>& nodePoses) const
{
    if (nodePoses.size() != _model->GetPoseNodes().size())
    {
        nodePoses.resize(_model->GetPoseNodes().size());
    }
    for (size_t nodeIndex = 0; nodeIndex < nodePoses.size(); ++nodeIndex)
    {
        ComputeAnimation(animationIndex, static_cast<int>(nodeIndex), time, nodePoses.at(nodeIndex));
    }
}

// ブレンディング計算処理
std::vector<ModelResource::Node> Animator::ComputeBlending(
    const std::vector<ModelResource::Node>& pose0,
    const std::vector<ModelResource::Node>& pose1,
    float rate) const
{
    // サイズ確認
    assert(pose0.size() == pose1.size());

    std::vector<ModelResource::Node> result = pose1;

    size_t count = _model->GetPoseNodes().size();
    for (size_t i = 0; i < count; ++i)
    {
        const ModelResource::Node& node0 = pose0.at(i);
        const ModelResource::Node& node1 = pose1.at(i);
        ModelResource::Node& res = result.at(i);

        DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&node0.scale);
        DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&node1.scale);
        DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&node0.rotation);
        DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&node1.rotation);
        DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&node0.position);
        DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&node1.position);

        DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
        DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
        DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

        DirectX::XMStoreFloat3(&res.scale, S);
        DirectX::XMStoreFloat4(&res.rotation, R);
        DirectX::XMStoreFloat3(&res.position, T);
    }

    return result;
}

/// ルートモーション処理
void Animator::ComputeRootMotion(int animationIndex,
    float oldAnimSeconds, float currentAnimSeconds,
    int controlNodeIndex,
    std::vector<ModelResource::Node>& resultNodePose,
    Vector3& movement) const
{
    resultNodePose = _model->GetPoseNodes();
    movement = _VECTOR3_ZERO;

    std::vector<ModelResource::Node> oldNodes;
    std::vector<ModelResource::Node> currentNodes;
    // 指定のアニメーション姿勢を取得
    const ModelResource::Animation& animation = _model->GetResource()->GetAnimations().at(animationIndex);
    ComputeAnimation(animationIndex, oldAnimSeconds, oldNodes);
    ComputeAnimation(animationIndex, currentAnimSeconds, currentNodes);
    ComputeAnimation(animationIndex, currentAnimSeconds, resultNodePose);

    // 前フレームと今フレームのノードのローカル座標を取得
    const Vector3& oldRootNoodPos = oldNodes[controlNodeIndex].position;
    const Vector3& currentRootNoodPos = currentNodes[controlNodeIndex].position;

    // 移動量計算
    if (oldAnimSeconds > currentAnimSeconds)
    {
        // ループアニメーションで前回のアニメーション経過時間よりも今回のアニメーション経過時間の方が
        // 小さい時の処理

        // 前フレームからアニメーション終了までの移動量を算出
        //std::vector<ModelResource::Node> endNodePoses;
        //ComputeAnimation(animationIndex, animation.secondsLength, endNodePoses);
        //const Vector3& endRootNoodPos = endNodePoses[controlNodeIndex].position;
        //movement += endRootNoodPos - oldRootNoodPos;
        //// アニメーション開始から今フレームまでの移動量を算出
        //std::vector<ModelResource::Node> startNodePoses;
        //ComputeAnimation(animationIndex, 0.0f, startNodePoses);
        //const Vector3& startRootNoodPos = startNodePoses[controlNodeIndex].position;
        //movement += currentRootNoodPos - startRootNoodPos;
    }
    else
    {
        movement += currentRootNoodPos - oldRootNoodPos;
    }

    // 移動量を変換
    {
        const ModelResource::Node* rootNood = &resultNodePose[controlNodeIndex];
        DirectX::XMMATRIX ParentWorldT = DirectX::XMLoadFloat4x4(&rootNood->parent->worldTransform);
        movement = Vec3TransformNormal(movement, ParentWorldT);
    }

    // 腰骨の位置を初回の位置に置く
    {
        std::vector<ModelResource::Node> startNodePoses;
        ComputeAnimation(animationIndex, 0.0f, startNodePoses);
        resultNodePose[controlNodeIndex].position = startNodePoses[controlNodeIndex].position;
    }
}

/// アニメーション名から番号取得
int Animator::GetAnimationIndex(const std::string& key) const
{
    const size_t animationSize = _model->GetResource()->GetAnimations().size();
    for (size_t i = 0; i < animationSize; ++i)
    {
        if (_model->GetResource()->GetAnimations().at(i).name == key)
            return static_cast<int>(i);
    }
    assert(!"アニメーションがありません");
    return -1;
}