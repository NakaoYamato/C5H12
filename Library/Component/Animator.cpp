#include "Animator.h"

#include "../../Library/Scene/Scene.h"
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

    _rootMovement = Vector3::Zero;
    std::vector<ModelResource::Node>&   poseNodes = _model.lock()->GetPoseNodes();
	std::vector<ModelResource::Node>    oldPoseNodes = poseNodes;

    // アニメーション経過時間更新
    UpdateAnimSeconds(elapsedTime);

    // アニメーション計算処理
    ComputeAnimation(_animationIndex, _animationTimer, poseNodes);

    // ブレンディング計算処理
    if (_blendTimer < _blendEndTime)
    {
        // ブレンド率計算
        float rate = _blendTimer / _blendEndTime;

        // ブレンド計算
		BlendPoseNode(oldPoseNodes, poseNodes, rate, poseNodes);

        // ノード設定
        _model.lock()->SetPoseNodes(poseNodes);

        // 時間経過
        _blendTimer += elapsedTime;
        if (_blendTimer >= _blendEndTime)
        {
            _blendTimer = _blendEndTime;
        }
    }

	// ルートモーション計算
    CalcRootMotion(elapsedTime, poseNodes);

    // 取り除き
    if (_rootNodeIndex != -1)
    {
        ModelResource::Node startRootNode{};
        ComputeAnimation(_animationIndex, _rootNodeIndex, 0.0f, startRootNode);
        // 回転量の取り除き
        if (_removeRootRotation)
        {
            // rootノードの回転量を取り除く
            poseNodes[_rootNodeIndex].rotation = startRootNode.rotation;
        }
		// 位置量の取り除き
		if (_removeRootMovement)
		{
			// rootノードの位置量を取り除く
			poseNodes[_rootNodeIndex].position = startRootNode.position;
		}
    }

    // アニメーションイベントの更新
    UpdateAnimationEvent();
}

// デバッグ表示
void Animator::DebugRender(const RenderContext& rc)
{
    // アニメーションを再生中は当たり判定を表示
    if (GetAnimationIndex() != -1)
    {
        // 当たり判定表示
        _animationEvent.DebugRender(GetAnimationName(), GetAnimationTimer(), GetActor()->GetTransform().GetMatrix());
    }
}

// GUI描画
void Animator::DrawGui()
{
    auto& animations = _model.lock()->GetResource()->GetAddressAnimations();
    static const char* optionNames[] =
    {
        u8"なし",
        u8"X位置除去",
        u8"Y位置除去",
        u8"Z位置除去",
        u8"XY位置除去",
        u8"XZ位置除去",
        u8"YZ位置除去",
        u8"XYZ位置除去",
        u8"オフセット使用",
    };

    ImGui::Checkbox(u8"ルートモーションするか", &_useRootMotion);
    ImGui::Checkbox(u8"ルートの回転量を取り除くか", &_removeRootRotation);
	ImGui::Checkbox(u8"ルートの位置量を取り除くか", &_removeRootMovement);
    ImGui::Combo(u8"ルートモーションノード", &_rootNodeIndex, _nodeNames.data(), (int)_nodeNames.size());
    int option = static_cast<int>(_rootMotionOption);
    if (ImGui::Combo(u8"ルートモーションオプション", &option, optionNames, _countof(optionNames)))
        _rootMotionOption = static_cast<RootMotionOption>(option);
    ImGui::DragFloat3(u8"移動量オフセット", &_rootOffset.x, 0.01f, -100.0f, 100.0f);
    if (_animationIndex >= 0)
    {
        auto& currentAnimation = animations[_animationIndex];
        ImGui::InputText(u8"再生中のアニメーション", &animations[_animationIndex].name, ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SliderFloat(u8"経過時間", &_animationTimer, 0.0f, currentAnimation.secondsLength);
        ImGui::Separator();
    }
    ImGui::DragFloat(u8"ブレンド時間", &_blendSeconds, 0.01f);
    if (ImGui::Button(u8"再生"))
    {
        if (_animationIndex != -1)
            this->PlayAnimation(_animationIndex, _isLoop, _blendSeconds);
    }
    ImGui::SameLine();
    ImGui::Checkbox(u8"ループ", &_isLoop);
    if (ImGui::TreeNode(u8"アニメーション"))
    {

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

    if (ImGui::TreeNode(u8"イベント"))
    {
        // メッセージリストの編集
        _animationEvent.DrawMassageListGui(false);
        ImGui::Separator();
        if (GetAnimationIndex() != -1)
            _animationEvent.DrawGui(GetAnimationName(), false);
        else
            _animationEvent.DrawGui(false);

        ImGui::TreePop();
    }
}

#pragma region アニメーション制御
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
    _activeAttackEvent = true;

    // ブレンドアニメーションパラメーター
    _blendTimer = 0.0f;
    _blendEndTime = blendSeconds;
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

/// ブレンディング計算処理
void Animator::BlendPoseNode(
    const std::vector<ModelResource::Node>& pose0, 
    const std::vector<ModelResource::Node>& pose1,
    float rate, 
    std::vector<ModelResource::Node>& result)const
{
    // サイズ確認
    assert(pose0.size() == pose1.size());

    size_t count = pose1.size();
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
}
/// 現在のアニメーションの回転量を取り除く
Quaternion Animator::RemoveRootRotation(int rootIndex)
{
    auto& poseNodes = _model.lock()->GetPoseNodes();
    ModelResource::Node startRootNode{};
    ComputeAnimation(_animationIndex, rootIndex, 0.0f, startRootNode);
    // 回転量の差分を求める
    Quaternion q = Quaternion::Multiply(
        Quaternion::Inverse(poseNodes[rootIndex].rotation),
        startRootNode.rotation);
    // ルートの回転量を取り除く
    poseNodes[rootIndex].rotation = startRootNode.rotation;
    return q;
}
#pragma endregion

#pragma region アニメーションイベント
/// 再生中のアニメーションのイベントを取得
std::vector<AnimationEvent::EventData> Animator::GetCurrentEvents()
{
	if (_animationIndex == -1)
        return std::vector<AnimationEvent::EventData>();

    return _animationEvent.GetCurrentEventData(
		GetAnimationName(), _animationTimer);

    return std::vector<AnimationEvent::EventData>();
}
#pragma endregion

#pragma region アクセサ
/// ルートモーションで使うノード番号設定
void Animator::SetRootNodeIndex(const std::string& key)
{
    if (!_model.lock())
        return;
    // モデルからノード番号取得
    SetRootNodeIndex(_model.lock()->GetNodeIndex(key));
}

/// モデルをリセット
void Animator::ResetModel(std::shared_ptr<Model> model)
{
    // モデルが設定されていなければ処理しない
    assert(model != nullptr);
	_model = model;

    _nodeNames.clear();
    // ノードの名前を全取得
    for (auto& node : model->GetPoseNodes())
    {
        _nodeNames.push_back(node.name.c_str());
    }

    // アニメーションイベント読み込み
	_animationEvent.Load(model);
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
#pragma endregion

/// ルートモーション計算
void Animator::CalcRootMotion(float elapsedTime, std::vector<ModelResource::Node>& poseNodes)
{
    if (!_useRootMotion) return;
    if (_rootNodeIndex == -1) return;
    if (!_isPlaying) return;

    // 現在のルートノード取得
    ModelResource::Node currentRootNode = poseNodes[_rootNodeIndex];
    // ブレンド中はブレンドしていないルートノードを取得
    if (_blendTimer < _blendEndTime)
    {
        ModelResource::Node node{};
        ComputeAnimation(_animationIndex, _rootNodeIndex, _animationTimer, node);
        // poseNodesの行列を使いたいためpositionのみ取得
        currentRootNode.position = node.position;
    }

    // 前フレームのルートノード取得
    float oldTimer = std::max<float>(_animationTimer - elapsedTime, 0.0f);
    ModelResource::Node oldRootNode{};
    ComputeAnimation(_animationIndex, _rootNodeIndex, oldTimer, oldRootNode);

    // ノードのワールド座標を取得
    Vector3 currentPosition = Vector3::TransformCoord(currentRootNode.position, currentRootNode.parent->worldTransform);
    Vector3 oldPosition = Vector3::TransformCoord(oldRootNode.position, currentRootNode.parent->worldTransform);

    // 移動量取得
    // デバッグでF5を押しているときは移動量を取得しない
    if (!Debug::Input::IsActive(DebugInput::BTN_F5))
        _rootMovement = currentPosition - oldPosition;
    // ポーズノードの移動量を取り除く
    poseNodes[_rootNodeIndex].position = {};
    ModelResource::Node startRootNode{};
    ComputeAnimation(_animationIndex, _rootNodeIndex, 0.0f, startRootNode);
    // オブジェクトに応じてノードの位置を調整
    switch (_rootMotionOption)
    {
    case Animator::RootMotionOption::None:
        break;
    case Animator::RootMotionOption::RemovePositionX:
        poseNodes[_rootNodeIndex].position.y = startRootNode.position.y;
        poseNodes[_rootNodeIndex].position.z = startRootNode.position.z;
        break;
    case Animator::RootMotionOption::RemovePositionY:
        poseNodes[_rootNodeIndex].position.x = startRootNode.position.x;
        poseNodes[_rootNodeIndex].position.z = startRootNode.position.z;
        break;
    case Animator::RootMotionOption::RemovePositionZ:
        poseNodes[_rootNodeIndex].position.x = startRootNode.position.x;
        poseNodes[_rootNodeIndex].position.y = startRootNode.position.y;
        break;
    case Animator::RootMotionOption::RemovePositionXY:
        poseNodes[_rootNodeIndex].position.z = startRootNode.position.z;
        break;
    case Animator::RootMotionOption::RemovePositionXZ:
        poseNodes[_rootNodeIndex].position.y = startRootNode.position.y;
        break;
    case Animator::RootMotionOption::RemovePositionYZ:
        poseNodes[_rootNodeIndex].position.x = startRootNode.position.x;
        break;
    case Animator::RootMotionOption::RemovePositionXYZ:
        break;
    case Animator::RootMotionOption::UseOffset:
    default:
        poseNodes[_rootNodeIndex].position = _rootOffset;
        break;
    }

    // モデルの姿勢を更新
    _model.lock()->SetPoseNodes(poseNodes);
}

/// アニメーションイベントの更新
void Animator::UpdateAnimationEvent()
{
    // 再生中でなければ処理しない
    if (!_isPlaying)
        return;

    for (auto& event : GetCurrentEvents())
    {
		if (event.eventType == AnimationEvent::EventType::Flag)
			continue;

        auto& collisionManager = GetActor()->GetScene()->GetCollisionManager();

        auto& node = _model.lock()->GetPoseNodes()[event.nodeIndex];
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&event.position));
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(event.angle.x, event.angle.y, event.angle.z);
        DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&event.scale));
        DirectX::XMFLOAT4X4 transform = {};

        DirectX::XMStoreFloat4x4(&transform, S * R * T * DirectX::XMLoadFloat4x4(&node.worldTransform));

		CollisionLayer layer = CollisionLayer::None;
		CollisionLayerMask layerMask = CollisionLayerMaskAll;
        if (event.eventType == AnimationEvent::EventType::Attack)
        {
            // 攻撃イベント使用フラグがオフなら処理しない
            if (!_activeAttackEvent)
                continue;
			layer = CollisionLayer::Attack;
            // 被弾レイヤー以外に当たらないようにする
			layerMask = GetCollisionLayerMask(CollisionLayer::Hit);
        }
		else if (event.eventType == AnimationEvent::EventType::Hit)
		{
			layer = CollisionLayer::Hit;
		}

        // 判定生成
        switch (event.shapeType)
        {
        case AnimationEvent::ShapeType::Sphere:
            collisionManager.RegisterSphereData(
                GetActor().get(),
                layer,
                layerMask,
                Vector3(transform._41, transform._42, transform._43),
                event.scale.x);
            break;
        case AnimationEvent::ShapeType::Box:
            collisionManager.RegisterBoxData(
                GetActor().get(),
                layer,
                layerMask,
                event.position.TransformCoord(node.worldTransform),
                event.scale,
                event.angle);
            break;
        case AnimationEvent::ShapeType::Capsule:
            collisionManager.RegisterCapsuleData(
                GetActor().get(),
                layer,
                layerMask,
                event.position.TransformCoord(node.worldTransform),
                event.angle.TransformCoord(node.worldTransform),
                event.scale.x);
            break;
        }
    }
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
