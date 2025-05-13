#include "Animator.h"

#include "../../Library/JobSystem/JobSystem.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 開始処理
void Animator::Start()
{
    ResetModel(GetActor()->GetModel().lock());
}

// 更新処理
void Animator::Update(float elapsedTime)
{
    // アニメーションが設定されていなければ処理しない
    if (_animationIndex == -1)
        return;

    std::vector<ModelResource::Node>& poseNodes = _model.lock()->GetPoseNodes();

    // アニメーション経過時間更新
    UpdateAnimSeconds(elapsedTime);

    // アニメーション計算処理
    ComputeAnimation(_animationIndex, _animationTimer, poseNodes);

  //  if (_removeMovement && _removeMovementNodeIndex != -1)
  //  {
		//auto& node = poseNode[_removeMovementNodeIndex];
		//node.position = Vector3::Zero;
  //  }

    // ブレンディング計算処理
    if (_isBlending)
    {
        // ブレンド率計算
        float rate = _blendTimer / _blendEndTime;

        poseNodes = ComputeBlending(_cacheNodes, _model.lock()->GetPoseNodes(), rate);
        // ノード設定
        _model.lock()->SetPoseNodes(poseNodes);

        // 時間経過
        _blendTimer += elapsedTime;
        if (_blendTimer >= _blendEndTime)
        {
            _blendTimer = _blendEndTime;
            _isBlending = false;
        }
    }

	// ルートモーション計算
	_rootMovement = Vector3::Zero;
	if (_useRootMotion && _rootNodeIndex != -1 && _isPlaying)
	{
		float oldTimer = std::max<float>(_animationTimer - elapsedTime, 0.0f);
        
        ModelResource::Node oldRootNode{};
        ModelResource::Node currentRootNode = poseNodes[_rootNodeIndex];
		ComputeAnimation(_animationIndex, _rootNodeIndex, oldTimer, oldRootNode);

        Vector3 currentPosition = Vector3::TransformCoord(currentRootNode.position, currentRootNode.parent->worldTransform);
        Vector3 oldPosition = Vector3::TransformCoord(oldRootNode.position, currentRootNode.parent->worldTransform);

        // 移動量取得
        _rootMovement = currentPosition - oldPosition;
        Debug::Renderer::DrawSphere(currentPosition, 0.1f, Vector4::Green);
        // ポーズノードの移動量を取り除く
        poseNodes[_rootNodeIndex].position = _rootOffset;

        // モデルの姿勢を更新
        _model.lock()->SetPoseNodes(poseNodes);
	}
}

// GUI描画
void Animator::DrawGui()
{
    ImGui::Checkbox(u8"ルートモーションするか", &_useRootMotion);
    ImGui::Combo(u8"ルートモーションノード", &_rootNodeIndex, _nodeNames.data(), (int)_nodeNames.size());
    ImGui::Checkbox(u8"移動量を取り除くか", &_removeMovement);
    ImGui::Combo(u8"移動量を取り除くノード", &_removeMovementNodeIndex, _nodeNames.data(), (int)_nodeNames.size());
    ImGui::DragFloat3(u8"移動量オフセット", &_rootOffset.x, 0.01f, -100.0f, 100.0f);
    if (ImGui::TreeNode(u8"アニメーション"))
    {
        auto& animations = _model.lock()->GetResource()->GetAddressAnimations();
        if (_animationIndex >= 0)
        {
            auto& currentAnimation = animations[_animationIndex];
            ImGui::Text(u8"再生中のアニメーション:");
            ImGui::SameLine();
            ImGui::Text(animations[_animationIndex].name.c_str());
            ImGui::SliderFloat(u8"経過時間", &_animationTimer, 0.0f, currentAnimation.secondsLength);

            if (ImGui::Button(u8"再生"))
            {
                this->PlayAnimation(_animationIndex, _isLoop, _blendSeconds);
            }
            ImGui::Separator();
        }
        ImGui::DragFloat(u8"ブレンド時間", &_blendSeconds, 0.01f);
        ImGui::Checkbox(u8"再生", &_isPlaying);
        ImGui::SameLine();
        ImGui::Checkbox(u8"ループ", &_isLoop);
        ImGui::SameLine();
        ImGui::Checkbox(u8"ブレンドするか", &_isBlending);

        ImGui::Separator();
        // フィルター
        if (ImGui::InputText(u8"検索", &_filterStr))
        {
            Filtering(_filterStr);
        }
        ImGui::Separator();

        //　何もフィルターしていなければすべて表示
        if (_filterStr == "")
        {
            int index = 0;
            for (const ModelResource::Animation& animation : animations)
            {
                ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

                if (ImGui::TreeNodeEx(&animation, nodeFlags, animation.name.c_str()))
                {
                    // ダブルクリックで再生
                    if (ImGui::IsItemClicked())
                    {
                        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            this->PlayAnimation(index, _isLoop, _blendSeconds);
                        }
                    }

                    ImGui::TreePop();
                }

                index++;
            }
        }
        else
        {
            // フィルターしたアニメーションのみ表示
            for (int index : _displayAnimationIndices)
            {
                ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

                if (ImGui::TreeNodeEx(&animations[index], nodeFlags, animations[index].name.c_str()))
                {
                    // ダブルクリックで再生
                    if (ImGui::IsItemClicked())
                    {
                        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            this->PlayAnimation(index, _isLoop, _blendSeconds);
                        }
                    }

                    ImGui::TreePop();
                }
            }
        }

        ImGui::TreePop();
    }
}

// アニメーション経過時間更新
void Animator::UpdateAnimSeconds(float elapsedTime)
{
    // 再生中でなければ処理しない
    if (!_isPlaying)
        return;

    // 経過時間
    _animationTimer += elapsedTime;
    // 再生時間が終端時間を超えた時
    const ModelResource::Animation& animation = _model.lock()->GetResource()->GetAnimations().at(_animationIndex);
    if (_animationTimer > animation.secondsLength)
    {
        if (_isLoop)
        {
            // 再生時間を戻す
            _animationTimer -= animation.secondsLength;
        }
        else
        {
            // 再生時間を終了時間にする
            _animationTimer = animation.secondsLength;
            _isPlaying = false;
        }
    }
}

// アニメーション再生
void Animator::PlayAnimation(int index, bool loop, float blendSeconds)
{
    // フラグを再設定
    _animationIndex = index;
    _animationTimer = 0;
    _isLoop = loop;
    _isPlaying = true;

    // ブレンドアニメーションパラメーター
    _isBlending = blendSeconds > 0.0f;
    _blendTimer = 0.0f;
    _blendEndTime = blendSeconds;

    // 現在の姿勢をキャッシュする
    for (size_t i = 0; i < _model.lock()->GetPoseNodes().size(); ++i)
    {
        const ModelResource::Node& src = _model.lock()->GetPoseNodes().at(i);
        ModelResource::Node& dst = _cacheNodes.at(i);

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
        if (_animationIndex != index)return false;
    }
    if (_animationIndex < 0)return false;
    if (_animationIndex >= _model.lock()->GetResource()->GetAnimations().size()) return false;
    return _isPlaying;
}

// アニメーション計算処理
void Animator::ComputeAnimation(int animationIndex, int nodeIndex, float time, ModelResource::Node& nodePose) const
{
    // 指定のアニメーションデータを収集
    const ModelResource::Animation& animation = _model.lock()->GetResource()->GetAnimations().at(animationIndex);
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
    if (nodePoses.size() != _model.lock()->GetPoseNodes().size())
    {
        nodePoses.resize(_model.lock()->GetPoseNodes().size());
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

    size_t count = _model.lock()->GetPoseNodes().size();
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
    resultNodePose = _model.lock()->GetPoseNodes();
    movement = Vector3::Zero;

    std::vector<ModelResource::Node> oldNodes;
    std::vector<ModelResource::Node> currentNodes;
    // 指定のアニメーション姿勢を取得
    const ModelResource::Animation& animation = _model.lock()->GetResource()->GetAnimations().at(animationIndex);
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
        movement = Vector3::TransformNormal(movement, ParentWorldT);
    }

    // 腰骨の位置を初回の位置に置く
    {
        std::vector<ModelResource::Node> startNodePoses;
        ComputeAnimation(animationIndex, 0.0f, startNodePoses);
        resultNodePose[controlNodeIndex].position = startNodePoses[controlNodeIndex].position;
    }
}

void Animator::ResetModel(std::shared_ptr<Model> model)
{
    // モデルが設定されていなければ処理しない
    assert(model != nullptr);
	_model = model;
    // ノードキャッシュの生成
    _cacheNodes.resize(model->GetPoseNodes().size());

    _nodeNames.clear();
    // ノードの名前を全取得
    for (auto& node : model->GetPoseNodes())
    {
        _nodeNames.push_back(node.name.c_str());
    }
}

/// アニメーション名から番号取得
int Animator::GetAnimationIndex(const std::string& key) const
{
    const size_t animationSize = _model.lock()->GetResource()->GetAnimations().size();
    for (size_t i = 0; i < animationSize; ++i)
    {
        if (_model.lock()->GetResource()->GetAnimations().at(i).name == key)
            return static_cast<int>(i);
    }
    assert(!"アニメーションがありません");
    return -1;
}

/// アニメーション番号から名前取得
std::string Animator::GetAnimationName(int index) const
{
    assert(index < _model.lock()->GetResource()->GetAnimations().size());
    assert(index >= 0);

    return _model.lock()->GetResource()->GetAnimations()[index].name;
}

std::string Animator::GetAnimationName() const
{
    return GetAnimationName(_animationIndex);
}

/// アニメーションのデバッグ表示をフィルタ
void Animator::Filtering(std::string filterStr)
{
    // 表示用コンテナをクリア
    _displayAnimationIndices.clear();

    // モデルのアニメーション名がfilterStrを含むか確認
    int index = 0;
    for (const ModelResource::Animation& animation : _model.lock()->GetResource()->GetAddressAnimations())
    {
        // filterStrを含んでいたら表示用コンテナに追加
        if (animation.name.find(filterStr) != std::string::npos)
        {
            _displayAnimationIndices.push_back(index);
        }

        index++;
    }
}
