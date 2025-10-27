#include "NodeShaker.h"

#include <imgui.h>

void NodeShaker::Start()
{
    // ダメージアブルに紐づける
    auto damageable = GetActor()->GetComponent<Damageable>();
    if (damageable)
    {
        // ダメージを受けたときのコールバック設定
        damageable->SetOnDamageCallback([this](float damage, Vector3 hitPosition)
            {
                // シェイク開始
                StartShake(hitPosition, _damageShakeDuration, _damageShakeMagnitude);
            });
    }
}

void NodeShaker::LateUpdate(float elapsedTime)
{
    auto model = _model.lock();
    if (!model)
        return;

    if (_shakeDuration != 0.0f)
    {
        _shakeElapsed += elapsedTime;

        float rate = Easings::GetFunc(_shakeEasingType)(_shakeElapsed / _shakeDuration);

        if (_shakeElapsed >= _shakeDuration)
        {
            // シェイク終了
            _shakeDuration = 0.0f;
            _shakeElapsed = 0.0f;
            rate = 1.0f;
        }

        // シェイク計算
        auto& node = model->GetPoseNodes()[_nodeIndex];

        node.rotation = Quaternion::Slerp(_shakenRotation, _originalRotation, rate);

        model->UpdateNodeTransform(&node);
    }
}

void NodeShaker::DrawGui()
{
    Easings::DrawGui(_shakeEasingType);
    ImGui::Separator();
    ImGui::DragFloat(u8"シェイク時間", &_damageShakeDuration, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat(u8"シェイク大きさ", &_damageShakeMagnitude, 0.1f, 0.0f, 50.0f);
    ImGui::Separator();
    ImGui::DragFloat(u8"ダメージシェイク時間", &_damageShakeDuration, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat(u8"ダメージシェイク大きさ", &_damageShakeMagnitude, 0.1f, 0.0f, 50.0f);
}

// シェイク開始
void NodeShaker::StartShake(const Vector3& position, float duration, float magnitude)
{
    this->_shakeDuration = duration;
    this->_shakeMagnitude = magnitude;

    auto model = _model.lock();
    if (!model)
        return;
    auto& node = model->GetPoseNodes()[_nodeIndex];
    
    DirectX::XMMATRIX nodeWorld = DirectX::XMLoadFloat4x4(&node.worldTransform);
    DirectX::XMVECTOR nodeLocalRotationM = DirectX::XMLoadFloat4(&node.rotation);
    DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
    // ノードのワールド座標 (ワールド行列の4行目)
    DirectX::XMVECTOR nodeWorldPos = nodeWorld.r[3];

    // 攻撃方向ベクトル (ダメージ源 -> ノード) を計算し、正規化
    DirectX::XMVECTOR attackDirWorld = DirectX::XMVector3Normalize(
        DirectX::XMVectorSubtract(nodeWorldPos, posVec)
    );

    // ノードのワールド空間でのUpベクトル (ワールド行列の2行目)
    DirectX::XMVECTOR nodeUpWorld = DirectX::XMVector3Normalize(nodeWorld.r[1]);

    // リアクションの回転軸を計算 (攻撃方向とUpベクトルの外積)
    DirectX::XMVECTOR rotationAxisWorld = DirectX::XMVector3Cross(attackDirWorld, nodeUpWorld);
    rotationAxisWorld = DirectX::XMVector3Normalize(rotationAxisWorld);

    // 攻撃が真上/真下からの場合、Crossが0になる
    if (DirectX::XMVector3Equal(DirectX::XMVector3LengthSq(rotationAxisWorld), DirectX::g_XMZero))
    {
        // Upの代わりにRightベクトル(ワールド行列の1行目)を使って軸を再計算
        DirectX::XMVECTOR nodeRightWorld = DirectX::XMVector3Normalize(nodeWorld.r[0]);
        rotationAxisWorld = DirectX::XMVector3Cross(attackDirWorld, nodeRightWorld);
        rotationAxisWorld = DirectX::XMVector3Normalize(rotationAxisWorld);
    }

    // ワールド空間でのリアクション回転(delta)をクォータニオンとして作成
    DirectX::XMVECTOR deltaRotationWorld = DirectX::XMQuaternionRotationAxis(
        rotationAxisWorld,
        DirectX::XMConvertToRadians(this->_shakeMagnitude)
    );

    // 親のワールド回転を取得 ---

    DirectX::XMVECTOR parentWorldRot = DirectX::XMQuaternionIdentity(); // 親がいない場合は単位クォータニオン
    if (node.parent)
    {
        // 親のワールド行列をロード
        DirectX::XMMATRIX parentWorldMat = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);

        // (重要) 親の行列からスケールを除去して、純粋な回転クォータニオンを取得
        DirectX::XMVECTOR parentScale, parentRot, parentTrans;
        DirectX::XMMatrixDecompose(&parentScale, &parentRot, &parentTrans, parentWorldMat);

        parentWorldRot = parentRot;
    }

        // 親のワールド回転の「逆」クォータニオンを計算
    DirectX::XMVECTOR invParentWorldRot = DirectX::XMQuaternionInverse(parentWorldRot);


    // ノードのローカル回転を更新 ---

    // ノードの現在のワールド回転を計算 (Local * Parent)
    DirectX::XMVECTOR worldRotOld = DirectX::XMQuaternionMultiply(nodeLocalRotationM, parentWorldRot);

    // 新しいワールド回転を計算 (OldWorld * DeltaWorld)
    DirectX::XMVECTOR worldRotNew = DirectX::XMQuaternionMultiply(worldRotOld, deltaRotationWorld);

    // 新しいローカル回転を導出 (NewWorld * InvParent)
    DirectX::XMVECTOR newLocalRot = DirectX::XMQuaternionMultiply(worldRotNew, invParentWorldRot);

    // 結果を格納 ---
    newLocalRot = DirectX::XMQuaternionNormalize(newLocalRot); // 最後に正規化

    // 保存
    _originalRotation = node.rotation;
    DirectX::XMStoreFloat4(&_shakenRotation, newLocalRot);
}
